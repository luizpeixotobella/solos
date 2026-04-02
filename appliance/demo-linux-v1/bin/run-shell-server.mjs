#!/usr/bin/env node
import { createReadStream, existsSync } from 'node:fs';
import { stat, readFile } from 'node:fs/promises';
import http from 'node:http';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const root = path.resolve(process.env.SOLOS_ROOT || path.join(__dirname, '..', '..', 'app', 'shell', 'dist'));
const host = process.env.SOLOS_BIND_HOST || '127.0.0.1';
const port = Number(process.env.SOLOS_PORT || 8080);
const logLevel = process.env.SOLOS_LOG_LEVEL || 'info';

const contentTypes = new Map([
  ['.html', 'text/html; charset=utf-8'],
  ['.js', 'text/javascript; charset=utf-8'],
  ['.css', 'text/css; charset=utf-8'],
  ['.json', 'application/json; charset=utf-8'],
  ['.svg', 'image/svg+xml'],
  ['.png', 'image/png'],
  ['.jpg', 'image/jpeg'],
  ['.jpeg', 'image/jpeg'],
  ['.webp', 'image/webp'],
  ['.woff', 'font/woff'],
  ['.woff2', 'font/woff2'],
]);

function log(...args) {
  if (logLevel !== 'silent') console.log('[solos-shell]', ...args);
}

function send(res, status, body, headers = {}) {
  res.writeHead(status, headers);
  res.end(body);
}

async function resolvePath(urlPath) {
  const cleanPath = decodeURIComponent(urlPath.split('?')[0]);
  const requested = cleanPath === '/' ? '/index.html' : cleanPath;
  const candidate = path.normalize(path.join(root, requested));

  if (!candidate.startsWith(root)) {
    return { kind: 'blocked' };
  }

  if (existsSync(candidate)) {
    const info = await stat(candidate);
    if (info.isDirectory()) {
      const indexPath = path.join(candidate, 'index.html');
      if (existsSync(indexPath)) return { kind: 'file', filePath: indexPath };
    } else {
      return { kind: 'file', filePath: candidate };
    }
  }

  const spaFallback = path.join(root, 'index.html');
  if (existsSync(spaFallback)) {
    return { kind: 'file', filePath: spaFallback, spa: true };
  }

  return { kind: 'missing' };
}

const server = http.createServer(async (req, res) => {
  try {
    const result = await resolvePath(req.url || '/');

    if (result.kind === 'blocked') {
      return send(res, 403, 'Forbidden');
    }

    if (result.kind === 'missing') {
      return send(res, 404, 'Not found');
    }

    const ext = path.extname(result.filePath).toLowerCase();
    const contentType = contentTypes.get(ext) || 'application/octet-stream';
    const headers = {
      'Content-Type': contentType,
      'Cache-Control': result.spa ? 'no-cache' : 'public, max-age=300',
    };

    createReadStream(result.filePath)
      .on('error', async () => {
        const message = await readFile(result.filePath, 'utf8').catch(() => 'Internal Server Error');
        send(res, 500, message);
      })
      .pipe(res.writeHead(200, headers));
  } catch (error) {
    console.error('[solos-shell] request failed', error);
    send(res, 500, 'Internal Server Error');
  }
});

server.listen(port, host, () => {
  log(`serving ${root} at http://${host}:${port}`);
});
