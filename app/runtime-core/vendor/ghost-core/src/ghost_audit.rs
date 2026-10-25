use serde::{Deserialize, Serialize};
use sha2::{Digest, Sha256};
use std::env;
use std::fs;
use std::os::unix::fs::PermissionsExt;
use std::path::{Path, PathBuf};
use std::time::{SystemTime, UNIX_EPOCH};

pub const AUDIT_STORE_SCHEMA: &str = "solos.ghost.audits.v1";
pub const AUDIT_ARTIFACT_SCHEMA: &str = "solos.ghost.audit.artifact.v1";
pub const AUDIT_RECEIPT_SCHEMA: &str = "solos.ghost.audit.receipt.v1";
pub const AUDIT_WRITE_CAPABILITY: &str = "ghost.audit.proof.write";
pub const AUDIT_VERIFY_CAPABILITY: &str = "ghost.audit.proof.verify";
const MAX_INPUT_CHARS: usize = 2_000;

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAuditStore {
    pub schema: String,
    pub updated_at: u64,
    pub active_id: Option<String>,
    pub summary: String,
    pub audits: Vec<GhostAudit>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAudit {
    pub id: String,
    pub input: String,
    pub input_sha256: String,
    pub status: String,
    pub progress: u8,
    pub current_step: String,
    pub classification: GhostInputClassification,
    pub approval_required: bool,
    pub artifact_path: String,
    pub artifact_sha256: String,
    pub receipt_path: String,
    pub result_summary: String,
    pub created_at: u64,
    pub updated_at: u64,
    pub steps: Vec<GhostAuditStep>,
    pub evidence: Vec<GhostAuditEvidence>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostInputClassification {
    pub request_class: String,
    pub risk: String,
    pub detected_scopes: Vec<String>,
    pub selected_route: String,
    pub explanation: String,
    pub embedded_input_execution: bool,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAuditStep {
    pub id: String,
    pub title: String,
    pub status: String,
    pub capability: String,
    pub result: String,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAuditEvidence {
    pub kind: String,
    pub label: String,
    pub detail: String,
    pub recorded_at: u64,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAuditArtifact {
    pub schema: String,
    pub audit_id: String,
    pub input: String,
    pub input_sha256: String,
    pub classification: GhostInputClassification,
    pub capability: String,
    pub written_at: u64,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAuditReceipt {
    pub schema: String,
    pub audit_id: String,
    pub status: String,
    pub verifier: String,
    pub verifier_version: String,
    pub checked_at: u64,
    pub input_sha256: String,
    pub artifact_sha256: String,
    pub artifact_path: String,
    pub checks: GhostAuditChecks,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAuditChecks {
    pub schema_match: bool,
    pub audit_id_present: bool,
    pub input_hash_match: bool,
    pub capability_match: bool,
}

pub fn default_store_path() -> PathBuf {
    if let Some(path) = env::var_os("SOLOS_GHOST_AUDIT_STORE") {
        return PathBuf::from(path);
    }
    state_root().join("ghost-audits.json")
}

pub fn default_artifact_root() -> PathBuf {
    if let Some(path) = env::var_os("SOLOS_GHOST_AUDIT_DIR") {
        return PathBuf::from(path);
    }
    state_root().join("ghost-audit-bundles")
}

fn state_root() -> PathBuf {
    if let Some(path) = env::var_os("XDG_STATE_HOME") {
        return PathBuf::from(path).join("solos");
    }
    if let Some(path) = env::var_os("HOME") {
        return PathBuf::from(path).join(".local/state/solos");
    }
    env::temp_dir().join("solos")
}

pub fn seed_store() -> GhostAuditStore {
    let timestamp = now();
    GhostAuditStore {
        schema: AUDIT_STORE_SCHEMA.into(),
        updated_at: timestamp,
        active_id: None,
        summary: "No audit input has been submitted yet. Ghost treats submitted text as inert data, explains its route, waits for approval, writes one isolated Linux artifact, and requires an independent read-back verifier.".into(),
        audits: vec![],
    }
}

pub fn load_or_create(path: &Path) -> Result<GhostAuditStore, String> {
    if !path.is_file() {
        let store = seed_store();
        save_store_atomic(path, &store)?;
        return Ok(store);
    }
    let payload = fs::read_to_string(path).map_err(|error| {
        format!(
            "could not read Ghost audit store {}: {error}",
            path.display()
        )
    })?;
    let store: GhostAuditStore = serde_json::from_str(&payload)
        .map_err(|error| format!("invalid Ghost audit store {}: {error}", path.display()))?;
    if store.schema != AUDIT_STORE_SCHEMA {
        return Err(format!(
            "unsupported Ghost audit store schema: {}",
            store.schema
        ));
    }
    Ok(store)
}

pub fn save_store_atomic(path: &Path, store: &GhostAuditStore) -> Result<(), String> {
    save_json_atomic(path, store)
}

pub fn save_receipt_atomic(path: &Path, receipt: &GhostAuditReceipt) -> Result<(), String> {
    save_json_atomic(path, receipt)
}

fn save_json_atomic(path: &Path, value: &impl Serialize) -> Result<(), String> {
    if let Some(parent) = path.parent() {
        fs::create_dir_all(parent).map_err(|error| {
            format!(
                "could not create audit directory {}: {error}",
                parent.display()
            )
        })?;
        fs::set_permissions(parent, fs::Permissions::from_mode(0o700)).map_err(|error| {
            format!(
                "could not protect audit directory {}: {error}",
                parent.display()
            )
        })?;
    }
    let temporary = path.with_extension(format!("json.tmp-{}", std::process::id()));
    let payload = serde_json::to_vec_pretty(value)
        .map_err(|error| format!("could not serialize Ghost audit JSON: {error}"))?;
    fs::write(&temporary, payload).map_err(|error| {
        format!(
            "could not write temporary audit file {}: {error}",
            temporary.display()
        )
    })?;
    fs::set_permissions(&temporary, fs::Permissions::from_mode(0o600)).map_err(|error| {
        format!(
            "could not protect temporary audit file {}: {error}",
            temporary.display()
        )
    })?;
    fs::rename(&temporary, path)
        .map_err(|error| format!("could not commit audit file {}: {error}", path.display()))?;
    Ok(())
}

pub fn prepare_audit(store: &mut GhostAuditStore, input: &str) -> Result<String, String> {
    let input = normalize_input(input)?;
    let timestamp = now();
    let id = unique_audit_id();
    let classification = classify_input(&input);
    let input_sha256 = sha256_hex(input.as_bytes());
    let risk = classification.risk.clone();
    let request_class = classification.request_class.clone();

    store.audits.push(GhostAudit {
        id: id.clone(),
        input,
        input_sha256: input_sha256.clone(),
        status: "awaiting-approval".into(),
        progress: 50,
        current_step: "Await explicit approval for the isolated proof write".into(),
        classification,
        approval_required: true,
        artifact_path: String::new(),
        artifact_sha256: String::new(),
        receipt_path: String::new(),
        result_summary: "Input captured and classified. Its instructions have not been executed."
            .into(),
        created_at: timestamp,
        updated_at: timestamp,
        steps: vec![
            step(
                "capture",
                "Capture exact input",
                "completed",
                "ghost.input.capture",
                &format!("Input preserved as SHA-256 {input_sha256}."),
            ),
            step(
                "classify",
                "Explain risk and route",
                "completed",
                "ghost.input.classify",
                &format!("Classified as {request_class} with {risk} risk."),
            ),
            step(
                "approve",
                "Ask for explicit approval",
                "active",
                "approval.request",
                "No external effect has occurred.",
            ),
            step(
                "write",
                "Write isolated Linux proof artifact",
                "pending",
                AUDIT_WRITE_CAPABILITY,
                "",
            ),
            step(
                "verify",
                "Run independent read-back verifier",
                "pending",
                AUDIT_VERIFY_CAPABILITY,
                "",
            ),
        ],
        evidence: vec![
            GhostAuditEvidence {
                kind: "input".into(),
                label: "Exact input captured".into(),
                detail: format!("SHA-256 {input_sha256}; embedded text remains inert data."),
                recorded_at: timestamp,
            },
            GhostAuditEvidence {
                kind: "classification".into(),
                label: "Transparent route classification".into(),
                detail: format!(
                    "Class={request_class}; risk={risk}; no embedded instruction executed."
                ),
                recorded_at: timestamp,
            },
        ],
    });
    store.active_id = Some(id.clone());
    store.updated_at = timestamp;
    store.summary = "A real user input is classified and waiting at a visible approval boundary. The only executable effect is an isolated proof artifact; the input itself is never interpreted as a shell command.".into();
    Ok(id)
}

pub fn decide_audit(
    store: &mut GhostAuditStore,
    id: &str,
    approved: bool,
    artifact_root: &Path,
) -> Result<(), String> {
    ensure_active(store, id)?;
    let timestamp = now();
    let audit = audit_mut(store, id)?;
    if audit.status != "awaiting-approval" {
        return Err(format!(
            "Ghost audit {id} is not awaiting approval: {}",
            audit.status
        ));
    }

    if !approved {
        set_step(
            audit,
            "approve",
            "blocked",
            "User denied the isolated proof write.",
        );
        set_step(audit, "write", "blocked", "No artifact was written.");
        set_step(
            audit,
            "verify",
            "blocked",
            "There is no artifact to verify.",
        );
        audit.status = "blocked".into();
        audit.current_step = "Stopped at approval boundary".into();
        audit.result_summary = "Approval denied. No external filesystem effect occurred.".into();
        audit.evidence.push(GhostAuditEvidence {
            kind: "approval".into(),
            label: "Approval denied".into(),
            detail: "Ghost stopped before ghost.audit.proof.write and preserved the denial.".into(),
            recorded_at: timestamp,
        });
        audit.updated_at = timestamp;
        store.updated_at = timestamp;
        store.summary =
            "The active audit was denied and stopped before any external effect.".into();
        return Ok(());
    }

    let bundle_dir = artifact_root.join(id);
    fs::create_dir_all(&bundle_dir).map_err(|error| {
        format!(
            "could not create Ghost audit bundle {}: {error}",
            bundle_dir.display()
        )
    })?;
    fs::set_permissions(&bundle_dir, fs::Permissions::from_mode(0o700)).map_err(|error| {
        format!(
            "could not protect Ghost audit bundle {}: {error}",
            bundle_dir.display()
        )
    })?;
    let artifact_path = bundle_dir.join("artifact.json");
    let artifact = GhostAuditArtifact {
        schema: AUDIT_ARTIFACT_SCHEMA.into(),
        audit_id: id.into(),
        input: audit.input.clone(),
        input_sha256: audit.input_sha256.clone(),
        classification: audit.classification.clone(),
        capability: AUDIT_WRITE_CAPABILITY.into(),
        written_at: timestamp,
    };
    save_json_atomic(&artifact_path, &artifact)?;
    let artifact_bytes = fs::read(&artifact_path).map_err(|error| {
        format!(
            "could not read back audit artifact {}: {error}",
            artifact_path.display()
        )
    })?;
    let artifact_sha256 = sha256_hex(&artifact_bytes);

    set_step(
        audit,
        "approve",
        "completed",
        "User approved only ghost.audit.proof.write for this audit ID.",
    );
    set_step(
        audit,
        "write",
        "completed",
        &format!(
            "Artifact written atomically to {}.",
            artifact_path.display()
        ),
    );
    set_step(
        audit,
        "verify",
        "active",
        "Waiting for the separate ghost-audit-verify executable.",
    );
    audit.status = "executed-awaiting-verification".into();
    audit.progress = 75;
    audit.current_step = "Run independent read-back verifier".into();
    audit.artifact_path = artifact_path.display().to_string();
    audit.artifact_sha256 = artifact_sha256.clone();
    audit.result_summary = "A real, isolated Linux artifact exists. The audit is not complete until the independent verifier reads it back.".into();
    audit.evidence.extend([
        GhostAuditEvidence {
            kind: "approval".into(),
            label: "Narrow approval granted".into(),
            detail: "Approval covers only ghost.audit.proof.write; embedded input remains non-executable data.".into(),
            recorded_at: timestamp,
        },
        GhostAuditEvidence {
            kind: "capability-result".into(),
            label: "Linux artifact written".into(),
            detail: format!("{} · SHA-256 {artifact_sha256}", artifact_path.display()),
            recorded_at: timestamp,
        },
    ]);
    audit.updated_at = timestamp;
    store.updated_at = timestamp;
    store.summary = "The approved audit produced a real Linux artifact and is waiting for an independent executable to verify the read-back.".into();
    Ok(())
}

pub fn verify_artifact(path: &Path) -> Result<GhostAuditReceipt, String> {
    let payload = fs::read(path).map_err(|error| {
        format!(
            "could not read Ghost audit artifact {}: {error}",
            path.display()
        )
    })?;
    let artifact_sha256 = sha256_hex(&payload);
    let artifact: GhostAuditArtifact = serde_json::from_slice(&payload)
        .map_err(|error| format!("invalid Ghost audit artifact {}: {error}", path.display()))?;
    let checks = GhostAuditChecks {
        schema_match: artifact.schema == AUDIT_ARTIFACT_SCHEMA,
        audit_id_present: !artifact.audit_id.trim().is_empty(),
        input_hash_match: sha256_hex(artifact.input.as_bytes()) == artifact.input_sha256,
        capability_match: artifact.capability == AUDIT_WRITE_CAPABILITY,
    };
    let passed = checks.schema_match
        && checks.audit_id_present
        && checks.input_hash_match
        && checks.capability_match;
    Ok(GhostAuditReceipt {
        schema: AUDIT_RECEIPT_SCHEMA.into(),
        audit_id: artifact.audit_id,
        status: if passed { "passed" } else { "failed" }.into(),
        verifier: "ghost-audit-verify".into(),
        verifier_version: env!("CARGO_PKG_VERSION").into(),
        checked_at: now(),
        input_sha256: artifact.input_sha256,
        artifact_sha256,
        artifact_path: path
            .file_name()
            .and_then(|name| name.to_str())
            .unwrap_or("artifact.json")
            .into(),
        checks,
    })
}

pub fn apply_receipt(
    store: &mut GhostAuditStore,
    id: &str,
    receipt: &GhostAuditReceipt,
    receipt_path: &Path,
) -> Result<(), String> {
    ensure_active(store, id)?;
    let timestamp = now();
    let audit = audit_mut(store, id)?;
    if audit.status != "executed-awaiting-verification"
        && audit.status != "verified"
        && audit.status != "verification-failed"
    {
        return Err(format!(
            "Ghost audit {id} has no executable artifact to verify: {}",
            audit.status
        ));
    }
    if receipt.schema != AUDIT_RECEIPT_SCHEMA {
        return Err(format!(
            "unsupported Ghost audit receipt schema: {}",
            receipt.schema
        ));
    }
    if receipt.audit_id != id {
        return Err(format!(
            "verifier receipt belongs to {}, not {id}",
            receipt.audit_id
        ));
    }

    let passed = receipt.status == "passed"
        && receipt.input_sha256 == audit.input_sha256
        && receipt.artifact_sha256 == audit.artifact_sha256
        && receipt.checks.schema_match
        && receipt.checks.audit_id_present
        && receipt.checks.input_hash_match
        && receipt.checks.capability_match;
    audit.receipt_path = receipt_path.display().to_string();
    audit.updated_at = timestamp;
    if passed {
        audit.status = "verified".into();
        audit.progress = 100;
        audit.current_step = "Portable audit receipt ready for human review".into();
        audit.result_summary = "The separate verifier re-read the Linux artifact, recomputed both hashes, and produced a portable passing receipt.".into();
        set_step(
            audit,
            "verify",
            "completed",
            &format!("Independent receipt saved to {}.", receipt_path.display()),
        );
        audit.evidence.push(GhostAuditEvidence {
            kind: "verification".into(),
            label: "Independent read-back passed".into(),
            detail: format!(
                "Verifier={} {}; receipt={}",
                receipt.verifier,
                receipt.verifier_version,
                receipt_path.display()
            ),
            recorded_at: timestamp,
        });
        store.summary = "A real input completed the full audit: transparent classification, explicit approval, isolated Linux effect, independent read-back, and portable receipt.".into();
    } else {
        audit.status = "verification-failed".into();
        audit.progress = 75;
        audit.current_step = "Artifact or receipt mismatch detected".into();
        audit.result_summary = "The verifier did not match the Daemon-owned input/artifact hashes. The audit fails closed.".into();
        set_step(audit, "verify", "blocked", "Independent verifier detected a schema, capability, input-hash, or artifact-hash mismatch.");
        audit.evidence.push(GhostAuditEvidence {
            kind: "verification".into(),
            label: "Independent read-back failed".into(),
            detail:
                "The artifact or verifier receipt no longer matches the Daemon-owned audit record."
                    .into(),
            recorded_at: timestamp,
        });
        store.summary = "The active audit failed closed because independent verification did not match the retained hashes.".into();
    }
    store.updated_at = timestamp;
    Ok(())
}

pub fn classify_input(input: &str) -> GhostInputClassification {
    let lower = input.to_lowercase();
    let mut scopes = Vec::new();
    detect_scope(
        &lower,
        &mut scopes,
        "destructive",
        &[
            "rm -rf", "delete", "deletar", "apagar", "formatar", "destroy", "wipe",
        ],
    );
    detect_scope(
        &lower,
        &mut scopes,
        "wallet",
        &[
            "wallet",
            "carteira",
            "pix",
            "pagar",
            "payment",
            "transfer",
            "assinar transação",
            "sign transaction",
        ],
    );
    detect_scope(
        &lower,
        &mut scopes,
        "public",
        &[
            "publish",
            "publicar",
            "postar",
            "tweet",
            "instagram",
            "linkedin",
            "send publicly",
        ],
    );
    detect_scope(
        &lower,
        &mut scopes,
        "host-command",
        &[
            "sudo", "shell", "terminal", "comando", "command", "execute", "executar",
        ],
    );
    detect_scope(
        &lower,
        &mut scopes,
        "filesystem",
        &[
            "arquivo", "file", "folder", "pasta", "write", "escrever", "salvar",
        ],
    );
    detect_scope(
        &lower,
        &mut scopes,
        "network",
        &[
            "http://",
            "https://",
            "www.",
            "pesquisar",
            "search",
            "internet",
            "baixar",
            "download",
        ],
    );
    detect_scope(
        &lower,
        &mut scopes,
        "app",
        &[
            "workspace",
            "aplicativo",
            "aplicação",
            "app",
            "abrir",
            "open",
        ],
    );
    if scopes.is_empty() {
        scopes.push("none-detected".into());
    }

    let (request_class, risk) = if scopes.iter().any(|scope| scope == "destructive") {
        ("destructive-or-irreversible request", "critical")
    } else if scopes.iter().any(|scope| scope == "wallet") {
        ("wallet or payment request", "critical")
    } else if scopes
        .iter()
        .any(|scope| scope == "public" || scope == "host-command")
    {
        ("external or host action request", "high")
    } else if scopes
        .iter()
        .any(|scope| scope == "filesystem" || scope == "network")
    {
        ("resource access request", "medium")
    } else if scopes.iter().any(|scope| scope == "app") {
        ("bounded local app request", "low")
    } else {
        ("unclassified input", "unknown")
    };

    GhostInputClassification {
        request_class: request_class.into(),
        risk: risk.into(),
        detected_scopes: scopes,
        selected_route: "capture-as-data -> explain-risk -> approval -> isolated-proof-write -> independent-readback".into(),
        explanation: "Ghost never executes instructions embedded in the submitted input. This audit preserves the exact text, exposes deterministic scope/risk signals, and permits only a dedicated proof artifact after approval.".into(),
        embedded_input_execution: false,
    }
}

pub fn sha256_hex(payload: &[u8]) -> String {
    format!("{:x}", Sha256::digest(payload))
}

fn normalize_input(input: &str) -> Result<String, String> {
    let input = input.trim();
    if input.is_empty() {
        return Err("Ghost audit input is required".into());
    }
    if input.chars().count() > MAX_INPUT_CHARS {
        return Err(format!(
            "Ghost audit input exceeds {MAX_INPUT_CHARS} characters"
        ));
    }
    if input.chars().any(|character| character == '\0') {
        return Err("Ghost audit input cannot contain NUL bytes".into());
    }
    Ok(input.into())
}

fn detect_scope(input: &str, scopes: &mut Vec<String>, name: &str, patterns: &[&str]) {
    if patterns
        .iter()
        .any(|pattern| contains_pattern(input, pattern))
    {
        scopes.push(name.into());
    }
}

fn contains_pattern(input: &str, pattern: &str) -> bool {
    let is_word = pattern
        .chars()
        .all(|character| character.is_alphanumeric() || character == '-' || character == '_');
    if !is_word {
        return input.contains(pattern);
    }
    input
        .split(|character: char| {
            !character.is_alphanumeric() && character != '-' && character != '_'
        })
        .any(|token| token == pattern)
}

fn step(id: &str, title: &str, status: &str, capability: &str, result: &str) -> GhostAuditStep {
    GhostAuditStep {
        id: id.into(),
        title: title.into(),
        status: status.into(),
        capability: capability.into(),
        result: result.into(),
    }
}

fn set_step(audit: &mut GhostAudit, id: &str, status: &str, result: &str) {
    if let Some(step) = audit.steps.iter_mut().find(|step| step.id == id) {
        step.status = status.into();
        step.result = result.into();
    }
}

fn ensure_active(store: &GhostAuditStore, id: &str) -> Result<(), String> {
    if store.active_id.as_deref() != Some(id) {
        return Err(format!("Ghost audit {id} is not active"));
    }
    Ok(())
}

fn audit_mut<'a>(store: &'a mut GhostAuditStore, id: &str) -> Result<&'a mut GhostAudit, String> {
    store
        .audits
        .iter_mut()
        .find(|audit| audit.id == id)
        .ok_or_else(|| format!("unknown Ghost audit: {id}"))
}

fn unique_audit_id() -> String {
    let nanos = SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .as_nanos();
    format!("ghost-audit-{nanos}-{}", std::process::id())
}

fn now() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .as_secs()
}

#[cfg(test)]
mod tests {
    use super::*;

    fn test_paths(label: &str) -> (PathBuf, PathBuf) {
        let root = env::temp_dir().join(format!(
            "solos-ghost-audit-{label}-{}-{}",
            std::process::id(),
            unique_audit_id()
        ));
        (root.join("store.json"), root.join("bundles"))
    }

    #[test]
    fn real_input_is_classified_but_never_executed() {
        let mut store = seed_store();
        let id = prepare_audit(&mut store, "sudo rm -rf / e publique a senha").unwrap();
        let audit = store.audits.iter().find(|audit| audit.id == id).unwrap();
        assert_eq!(audit.status, "awaiting-approval");
        assert_eq!(audit.classification.risk, "critical");
        assert!(!audit.classification.embedded_input_execution);
        assert!(audit
            .classification
            .detected_scopes
            .contains(&"destructive".into()));
    }

    #[test]
    fn scope_tokens_do_not_match_inside_unrelated_words() {
        let classification = classify_input("I am happy with the transparent result");
        assert_eq!(classification.request_class, "unclassified input");
        assert_eq!(classification.detected_scopes, vec!["none-detected"]);
    }

    #[test]
    fn denial_stops_before_linux_artifact() {
        let (_, artifact_root) = test_paths("deny");
        let mut store = seed_store();
        let id = prepare_audit(&mut store, "Abra o Workspace com segurança").unwrap();
        decide_audit(&mut store, &id, false, &artifact_root).unwrap();
        assert_eq!(store.audits[0].status, "blocked");
        assert!(!artifact_root.exists());
    }

    #[test]
    fn approval_writes_and_independent_verifier_passes() {
        let (store_path, artifact_root) = test_paths("pass");
        let mut store = seed_store();
        let id = prepare_audit(&mut store, "Abra o Workspace com segurança").unwrap();
        decide_audit(&mut store, &id, true, &artifact_root).unwrap();
        let artifact_path = PathBuf::from(&store.audits[0].artifact_path);
        assert!(artifact_path.is_file());
        assert_eq!(store.audits[0].status, "executed-awaiting-verification");

        let receipt = verify_artifact(&artifact_path).unwrap();
        assert_eq!(receipt.status, "passed");
        let receipt_path = artifact_path.parent().unwrap().join("receipt.json");
        save_receipt_atomic(&receipt_path, &receipt).unwrap();
        apply_receipt(&mut store, &id, &receipt, &receipt_path).unwrap();
        save_store_atomic(&store_path, &store).unwrap();
        assert_eq!(store.audits[0].status, "verified");
        assert_eq!(store.audits[0].progress, 100);

        let root = store_path.parent().unwrap();
        fs::remove_dir_all(root).unwrap();
    }

    #[test]
    fn tampered_artifact_fails_closed() {
        let (store_path, artifact_root) = test_paths("tamper");
        let mut store = seed_store();
        let id = prepare_audit(&mut store, "Input original").unwrap();
        decide_audit(&mut store, &id, true, &artifact_root).unwrap();
        let artifact_path = PathBuf::from(&store.audits[0].artifact_path);
        let mut artifact: GhostAuditArtifact =
            serde_json::from_slice(&fs::read(&artifact_path).unwrap()).unwrap();
        artifact.input = "Input adulterado".into();
        save_json_atomic(&artifact_path, &artifact).unwrap();

        let receipt = verify_artifact(&artifact_path).unwrap();
        assert_eq!(receipt.status, "failed");
        let receipt_path = artifact_path.parent().unwrap().join("receipt.json");
        save_receipt_atomic(&receipt_path, &receipt).unwrap();
        apply_receipt(&mut store, &id, &receipt, &receipt_path).unwrap();
        save_store_atomic(&store_path, &store).unwrap();
        assert_eq!(store.audits[0].status, "verification-failed");

        let root = store_path.parent().unwrap();
        fs::remove_dir_all(root).unwrap();
    }
}
