#!/usr/bin/env node

import { createHmac } from "node:crypto";
import { mkdir, readFile, rename, writeFile, chmod } from "node:fs/promises";
import net from "node:net";
import os from "node:os";
import path from "node:path";

const endpoint = process.env.SOLOS_GHOST_CMS_ENDPOINT || "https://luiz-bella-artes.net/api/ghost/bridge";
const secret = process.env.SOLOS_GHOST_CMS_SECRET || "";
const runtimeDir = process.env.XDG_RUNTIME_DIR || path.join(os.tmpdir(), `solos-${process.getuid?.() ?? process.pid}`);
const socketPath = process.env.SOLOS_DAEMON_SOCKET || path.join(runtimeDir, "solos/daemon.sock");
const stateRoot = process.env.XDG_STATE_HOME || path.join(os.homedir(), ".local/state");
const cursorPath = process.env.SOLOS_GHOST_SYNC_CURSOR || path.join(stateRoot, "solos/ghost-sync-cursor.json");
const dryRun = process.argv.includes("--dry-run");

if (!endpoint.startsWith("https://") && !endpoint.startsWith("http://127.0.0.1:")) {
  throw new Error("SOLOS_GHOST_CMS_ENDPOINT must use HTTPS (or loopback HTTP for a local smoke test)");
}
if (!dryRun && secret.length < 32) {
  throw new Error("SOLOS_GHOST_CMS_SECRET must contain at least 32 characters");
}

async function loadCursor() {
  try {
    const value = JSON.parse(await readFile(cursorPath, "utf8"));
    return Number.isSafeInteger(value.lastSequence) && value.lastSequence >= 0 ? value.lastSequence : 0;
  } catch (error) {
    if (error?.code === "ENOENT") return 0;
    throw error;
  }
}

async function saveCursor(lastSequence) {
  await mkdir(path.dirname(cursorPath), { recursive: true, mode: 0o700 });
  const temporary = `${cursorPath}.tmp-${process.pid}`;
  await writeFile(temporary, `${JSON.stringify({ schema: "solos.ghost.sync-cursor.v1", lastSequence })}\n`, { mode: 0o600 });
  await chmod(temporary, 0o600);
  await rename(temporary, cursorPath);
}

function daemonRpc(method, params = {}) {
  return new Promise((resolve, reject) => {
    const client = net.createConnection(socketPath);
    let buffer = "";
    const timeout = setTimeout(() => client.destroy(new Error("SolOS Daemon RPC timed out")), 5_000);
    client.setEncoding("utf8");
    client.once("connect", () => {
      client.write(`${JSON.stringify({ id: `ghost-sync-${process.pid}`, method, params })}\n`);
    });
    client.on("data", (chunk) => {
      buffer += chunk;
      const newline = buffer.indexOf("\n");
      if (newline < 0) return;
      clearTimeout(timeout);
      client.end();
      try {
        const response = JSON.parse(buffer.slice(0, newline));
        if (!response.ok) reject(new Error(response.error || "SolOS Daemon rejected the request"));
        else resolve(response.result);
      } catch (error) {
        reject(error);
      }
    });
    client.once("error", (error) => {
      clearTimeout(timeout);
      reject(error);
    });
  });
}

const afterSequence = await loadCursor();
const exported = await daemonRpc("events.export", { afterSequence, limit: 100 });
if (exported.schema !== "solos.ghost.brain-export.v1" || !Array.isArray(exported.events)) {
  throw new Error("SolOS Daemon returned an unsupported brain export");
}
if (exported.events.length === 0) {
  console.log(JSON.stringify({ status: "empty", afterSequence }));
  process.exit(0);
}

const body = JSON.stringify({
  schema: "solos.ghost.brain-ingest.v1",
  events: exported.events,
});

if (dryRun) {
  console.log(JSON.stringify({
    status: "dry-run",
    eventCount: exported.events.length,
    firstSequence: exported.events[0]?.metrics?.sequence,
    lastSequence: exported.lastSequence,
  }));
  process.exit(0);
}

const timestamp = Math.floor(Date.now() / 1_000).toString();
const signature = createHmac("sha256", secret).update(`${timestamp}.${body}`, "utf8").digest("hex");
const response = await fetch(endpoint, {
  method: "POST",
  headers: {
    "content-type": "application/json",
    "x-solos-timestamp": timestamp,
    "x-solos-signature": `sha256=${signature}`,
  },
  body,
});
if (!response.ok) {
  const detail = (await response.text()).slice(0, 500);
  throw new Error(`Ghost CMS bridge rejected the batch (${response.status}): ${detail}`);
}
const result = await response.json();
await saveCursor(exported.lastSequence);
console.log(JSON.stringify({
  status: "synced",
  sent: exported.events.length,
  inserted: Number(result.inserted || 0),
  lastSequence: exported.lastSequence,
}));
