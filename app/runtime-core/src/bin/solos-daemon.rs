use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use solos_runtime_core::ghost_audit::{
    apply_receipt, decide_audit, default_artifact_root,
    default_store_path as default_audit_store_path, load_or_create as load_or_create_audit_store,
    prepare_audit, save_receipt_atomic, save_store_atomic as save_audit_store_atomic,
    GhostAuditReceipt, GhostAuditStore,
};
use solos_runtime_core::ghost_resolution::{
    decide_resolution, default_store_path, load_or_create, reset_store, save_atomic,
    select_resolution, start_resolution, GhostResolutionStore,
};
use std::collections::VecDeque;
use std::env;
use std::fs;
use std::io::{BufRead, BufReader, Write};
use std::os::unix::fs::PermissionsExt;
use std::os::unix::net::{UnixListener, UnixStream};
use std::path::{Path, PathBuf};
use std::process::Command;
use std::sync::{Arc, Mutex};
use std::time::{SystemTime, UNIX_EPOCH};

const PROTOCOL: &str = "solos.daemon.protocol.v1";
const EVENT_LIMIT: usize = 256;

#[derive(Debug, Deserialize)]
struct Request {
    #[serde(default)]
    id: Option<String>,
    method: String,
    #[serde(default)]
    params: Value,
}

#[derive(Serialize)]
struct Response {
    protocol: &'static str,
    id: Option<String>,
    ok: bool,
    result: Value,
    error: Option<String>,
}

#[derive(Clone)]
struct DaemonState {
    started_at: u64,
    snapshot_path: PathBuf,
    resolution_path: PathBuf,
    audit_path: PathBuf,
    audit_root: PathBuf,
    audit_verifier_path: PathBuf,
    resolution_lock: Arc<Mutex<()>>,
    audit_lock: Arc<Mutex<()>>,
    events: Arc<Mutex<VecDeque<Value>>>,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let socket_path = env::var_os("SOLOS_DAEMON_SOCKET")
        .map(PathBuf::from)
        .unwrap_or_else(|| {
            env::var_os("XDG_RUNTIME_DIR")
                .map(PathBuf::from)
                .unwrap_or_else(|| env::temp_dir().join(format!("solos-{}", std::process::id())))
                .join("solos/daemon.sock")
        });
    let snapshot_path = env::var_os("SOLOS_RUNTIME_SNAPSHOT")
        .map(PathBuf::from)
        .unwrap_or_else(|| PathBuf::from("runtime_snapshot.json"));
    let resolution_path = default_store_path();
    let audit_path = default_audit_store_path();
    let audit_root = default_artifact_root();
    let audit_verifier_path = env::var_os("SOLOS_GHOST_AUDIT_VERIFIER")
        .map(PathBuf::from)
        .unwrap_or_else(|| {
            env::current_exe()
                .ok()
                .and_then(|path| path.parent().map(Path::to_path_buf))
                .unwrap_or_else(|| PathBuf::from("."))
                .join("ghost-audit-verify")
        });

    prepare_socket(&socket_path)?;
    let listener = UnixListener::bind(&socket_path)?;
    fs::set_permissions(&socket_path, fs::Permissions::from_mode(0o600))?;

    let state = DaemonState {
        started_at: now(),
        snapshot_path,
        resolution_path,
        audit_path,
        audit_root,
        audit_verifier_path,
        resolution_lock: Arc::new(Mutex::new(())),
        audit_lock: Arc::new(Mutex::new(())),
        events: Arc::new(Mutex::new(VecDeque::new())),
    };
    publish_event(&state, "daemon.started", json!({"socket": socket_path}))?;

    eprintln!("[solos-daemon] listening on {}", socket_path.display());
    for connection in listener.incoming() {
        match connection {
            Ok(stream) => {
                let state = state.clone();
                std::thread::spawn(move || {
                    if let Err(error) = serve_client(stream, &state) {
                        eprintln!("[solos-daemon] client error: {error}");
                    }
                });
            }
            Err(error) => eprintln!("[solos-daemon] accept error: {error}"),
        }
    }
    Ok(())
}

fn prepare_socket(path: &Path) -> std::io::Result<()> {
    if let Some(parent) = path.parent() {
        if !parent.exists() {
            fs::create_dir_all(parent)?;
            fs::set_permissions(parent, fs::Permissions::from_mode(0o700))?;
        }
    }
    if path.exists() {
        fs::remove_file(path)?;
    }
    Ok(())
}

fn serve_client(mut stream: UnixStream, state: &DaemonState) -> Result<(), String> {
    let reader_stream = stream.try_clone().map_err(|error| error.to_string())?;
    let mut reader = BufReader::new(reader_stream);
    loop {
        let mut line = String::new();
        if reader
            .read_line(&mut line)
            .map_err(|error| error.to_string())?
            == 0
        {
            return Ok(());
        }
        if line.len() > 1024 * 1024 {
            return Err("request exceeds 1 MiB".into());
        }
        let response = match serde_json::from_str::<Request>(&line) {
            Ok(request) => dispatch(request, state),
            Err(error) => failure(None, format!("invalid request: {error}")),
        };
        serde_json::to_writer(&mut stream, &response).map_err(|error| error.to_string())?;
        stream.write_all(b"\n").map_err(|error| error.to_string())?;
        stream.flush().map_err(|error| error.to_string())?;
    }
}

fn dispatch(request: Request, state: &DaemonState) -> Response {
    let result = match request.method.as_str() {
        "health.get" => Ok(json!({
            "status": "healthy",
            "role": "runtime-intermediary-daemon",
            "uptimeSeconds": now().saturating_sub(state.started_at),
            "snapshotAvailable": state.snapshot_path.is_file(),
            "snapshotPath": state.snapshot_path,
            "ghostAuditVerifierAvailable": state.audit_verifier_path.is_file(),
            "ghostAuditVerifierPath": state.audit_verifier_path,
        })),
        "snapshot.get" => read_snapshot(state),
        "ghost.audits.get" => with_audit_store(state, |store| Ok(json!(store))),
        "ghost.audit.prepare" => mutate_audit(state, "ghost.audit.awaiting-approval", |store| {
            let input = required_string(&request.params, "input")?;
            let id = prepare_audit(store, input)?;
            let audit = store
                .audits
                .iter()
                .find(|audit| audit.id == id)
                .ok_or_else(|| "prepared Ghost audit disappeared".to_string())?;
            Ok(json!({
                "auditId": id,
                "status": audit.status,
                "classification": audit.classification,
                "inputSha256": audit.input_sha256,
            }))
        }),
        "ghost.audit.decide" => mutate_audit(state, "ghost.audit.decided", |store| {
            let id = required_string(&request.params, "id")?;
            let approved = request
                .params
                .get("approved")
                .and_then(Value::as_bool)
                .ok_or_else(|| "approved must be a boolean".to_string())?;
            decide_audit(store, id, approved, &state.audit_root)?;
            let audit = store
                .audits
                .iter()
                .find(|audit| audit.id == id)
                .ok_or_else(|| "decided Ghost audit disappeared".to_string())?;
            Ok(json!({
                "auditId": id,
                "approved": approved,
                "status": audit.status,
                "artifactPath": audit.artifact_path,
                "artifactSha256": audit.artifact_sha256,
            }))
        }),
        "ghost.audit.verify" => {
            let id = required_string(&request.params, "id");
            id.and_then(|id| verify_audit_with_external_process(state, id))
        }
        "ghost.resolutions.get" => with_resolution_store(state, |store| Ok(json!(store))),
        "ghost.resolution.select" => {
            mutate_resolution(state, "ghost.resolution.selected", |store| {
                let id = required_resolution_id(&request.params)?;
                select_resolution(store, id)?;
                Ok(json!({"resolutionId": id, "status": "selected"}))
            })
        }
        "ghost.resolution.start" => {
            mutate_resolution(state, "ghost.resolution.awaiting-approval", |store| {
                let id = required_resolution_id(&request.params)?;
                start_resolution(store, id)?;
                Ok(json!({"resolutionId": id, "status": "awaiting-approval"}))
            })
        }
        "ghost.resolution.decide" => {
            mutate_resolution(state, "ghost.resolution.decided", |store| {
                let id = required_resolution_id(&request.params)?;
                let approved = request
                    .params
                    .get("approved")
                    .and_then(Value::as_bool)
                    .ok_or_else(|| "approved must be a boolean".to_string())?;
                decide_resolution(store, id, approved)?;
                let status = if approved { "resolved" } else { "blocked" };
                Ok(json!({"resolutionId": id, "approved": approved, "status": status}))
            })
        }
        "ghost.resolutions.reset" => mutate_resolution(state, "ghost.resolutions.reset", |store| {
            reset_store(store);
            Ok(json!({"status": "reset", "selectedId": store.selected_id}))
        }),
        "events.list" => state
            .events
            .lock()
            .map(|events| json!({"events": events.iter().cloned().collect::<Vec<_>>() }))
            .map_err(|_| "event store unavailable".to_string()),
        "event.publish" => {
            let kind = request.params.get("kind").and_then(Value::as_str);
            match kind {
                Some(kind) if kind.starts_with("shell.") || kind.starts_with("ghost.") => {
                    publish_event(
                        state,
                        kind,
                        request.params.get("data").cloned().unwrap_or(Value::Null),
                    )
                }
                _ => Err("event kind must use an allowed shell.* or ghost.* namespace".into()),
            }
        }
        _ => Err(format!("unknown method: {}", request.method)),
    };

    match result {
        Ok(value) => success(request.id, value),
        Err(error) => failure(request.id, error),
    }
}

fn read_snapshot(state: &DaemonState) -> Result<Value, String> {
    let content = fs::read_to_string(&state.snapshot_path).map_err(|error| {
        format!(
            "snapshot unavailable at {}: {error}",
            state.snapshot_path.display()
        )
    })?;
    let mut snapshot: Value = serde_json::from_str(&content)
        .map_err(|error| format!("invalid runtime snapshot: {error}"))?;
    let resolutions = with_resolution_store(state, |store| Ok(store.clone()))?;
    let ghost = snapshot
        .get_mut("ghost")
        .and_then(Value::as_object_mut)
        .ok_or_else(|| "runtime snapshot has no Ghost object".to_string())?;
    ghost.insert(
        "resolutionLoop".into(),
        serde_json::to_value(resolutions)
            .map_err(|error| format!("could not serialize Ghost resolutions: {error}"))?,
    );
    let audits = with_audit_store(state, |store| Ok(store.clone()))?;
    ghost.insert(
        "auditChallenge".into(),
        serde_json::to_value(audits)
            .map_err(|error| format!("could not serialize Ghost audits: {error}"))?,
    );
    Ok(snapshot)
}

fn required_string<'a>(params: &'a Value, key: &str) -> Result<&'a str, String> {
    params
        .get(key)
        .and_then(Value::as_str)
        .filter(|value| !value.trim().is_empty())
        .ok_or_else(|| format!("{key} is required"))
}

fn required_resolution_id(params: &Value) -> Result<&str, String> {
    params
        .get("id")
        .and_then(Value::as_str)
        .filter(|id| !id.trim().is_empty())
        .ok_or_else(|| "resolution id is required".to_string())
}

fn with_resolution_store<T>(
    state: &DaemonState,
    action: impl FnOnce(&GhostResolutionStore) -> Result<T, String>,
) -> Result<T, String> {
    let _guard = state
        .resolution_lock
        .lock()
        .map_err(|_| "resolution store lock unavailable".to_string())?;
    let store = load_or_create(&state.resolution_path)?;
    action(&store)
}

fn mutate_resolution(
    state: &DaemonState,
    event_kind: &str,
    action: impl FnOnce(&mut GhostResolutionStore) -> Result<Value, String>,
) -> Result<Value, String> {
    let (result, store) = {
        let _guard = state
            .resolution_lock
            .lock()
            .map_err(|_| "resolution store lock unavailable".to_string())?;
        let mut store = load_or_create(&state.resolution_path)?;
        let result = action(&mut store)?;
        save_atomic(&state.resolution_path, &store)?;
        (result, store)
    };
    publish_event(
        state,
        event_kind,
        json!({"result": result, "resolutionLoop": store}),
    )?;
    Ok(json!({"transition": result, "resolutionLoop": store}))
}

fn with_audit_store<T>(
    state: &DaemonState,
    action: impl FnOnce(&GhostAuditStore) -> Result<T, String>,
) -> Result<T, String> {
    let _guard = state
        .audit_lock
        .lock()
        .map_err(|_| "Ghost audit store lock unavailable".to_string())?;
    let store = load_or_create_audit_store(&state.audit_path)?;
    action(&store)
}

fn mutate_audit(
    state: &DaemonState,
    event_kind: &str,
    action: impl FnOnce(&mut GhostAuditStore) -> Result<Value, String>,
) -> Result<Value, String> {
    let (result, store) = {
        let _guard = state
            .audit_lock
            .lock()
            .map_err(|_| "Ghost audit store lock unavailable".to_string())?;
        let mut store = load_or_create_audit_store(&state.audit_path)?;
        let result = action(&mut store)?;
        save_audit_store_atomic(&state.audit_path, &store)?;
        (result, store)
    };
    publish_event(
        state,
        event_kind,
        json!({"result": result, "auditChallenge": store}),
    )?;
    Ok(json!({"transition": result, "auditChallenge": store}))
}

fn verify_audit_with_external_process(state: &DaemonState, id: &str) -> Result<Value, String> {
    let audit = with_audit_store(state, |store| {
        store
            .audits
            .iter()
            .find(|audit| audit.id == id)
            .cloned()
            .ok_or_else(|| format!("unknown Ghost audit: {id}"))
    })?;
    if audit.artifact_path.is_empty() {
        return Err(format!("Ghost audit {id} has no artifact to verify"));
    }
    if !state.audit_verifier_path.is_file() {
        return Err(format!(
            "independent Ghost audit verifier is unavailable at {}",
            state.audit_verifier_path.display()
        ));
    }

    let output = Command::new(&state.audit_verifier_path)
        .arg(&audit.artifact_path)
        .output()
        .map_err(|error| {
            format!(
                "could not run independent Ghost audit verifier {}: {error}",
                state.audit_verifier_path.display()
            )
        })?;
    let receipt: GhostAuditReceipt = serde_json::from_slice(&output.stdout).map_err(|error| {
        let stderr = String::from_utf8_lossy(&output.stderr);
        format!("independent Ghost audit verifier returned invalid JSON: {error}; {stderr}")
    })?;
    let artifact_path = PathBuf::from(&audit.artifact_path);
    let receipt_path = artifact_path
        .parent()
        .ok_or_else(|| "Ghost audit artifact has no bundle directory".to_string())?
        .join("receipt.json");
    save_receipt_atomic(&receipt_path, &receipt)?;

    mutate_audit(state, "ghost.audit.verified", |store| {
        apply_receipt(store, id, &receipt, &receipt_path)?;
        let current = store
            .audits
            .iter()
            .find(|audit| audit.id == id)
            .ok_or_else(|| "verified Ghost audit disappeared".to_string())?;
        Ok(json!({
            "auditId": id,
            "status": current.status,
            "receiptPath": current.receipt_path,
            "receipt": receipt,
            "verifierExitCode": output.status.code(),
        }))
    })
}

fn publish_event(state: &DaemonState, kind: &str, data: Value) -> Result<Value, String> {
    let event = json!({
        "schema": "solos.daemon.event.v1",
        "kind": kind,
        "timestamp": now(),
        "data": data,
    });
    let mut events = state.events.lock().map_err(|_| "event store unavailable")?;
    if events.len() == EVENT_LIMIT {
        events.pop_front();
    }
    events.push_back(event.clone());
    Ok(event)
}

fn success(id: Option<String>, result: Value) -> Response {
    Response {
        protocol: PROTOCOL,
        id,
        ok: true,
        result,
        error: None,
    }
}

fn failure(id: Option<String>, error: String) -> Response {
    Response {
        protocol: PROTOCOL,
        id,
        ok: false,
        result: Value::Null,
        error: Some(error),
    }
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

    fn state() -> DaemonState {
        let unique = SystemTime::now()
            .duration_since(UNIX_EPOCH)
            .unwrap_or_default()
            .as_nanos();
        let root =
            env::temp_dir().join(format!("solos-daemon-test-{}-{unique}", std::process::id()));
        let resolution_path = root.join("ghost-resolutions.json");
        DaemonState {
            started_at: now(),
            snapshot_path: PathBuf::from("/path/that/does/not/exist"),
            resolution_path,
            audit_path: root.join("ghost-audits.json"),
            audit_root: root.join("audit-bundles"),
            audit_verifier_path: root.join("ghost-audit-verify"),
            resolution_lock: Arc::new(Mutex::new(())),
            audit_lock: Arc::new(Mutex::new(())),
            events: Arc::new(Mutex::new(VecDeque::new())),
        }
    }

    #[test]
    fn health_is_available_without_snapshot() {
        let response = dispatch(
            Request {
                id: Some("1".into()),
                method: "health.get".into(),
                params: Value::Null,
            },
            &state(),
        );
        assert!(response.ok);
        assert_eq!(response.result["status"], "healthy");
        assert_eq!(response.result["snapshotAvailable"], false);
    }

    #[test]
    fn event_namespaces_are_restricted() {
        let response = dispatch(
            Request {
                id: None,
                method: "event.publish".into(),
                params: json!({"kind": "wallet.transfer", "data": {}}),
            },
            &state(),
        );
        assert!(!response.ok);
    }

    #[test]
    fn event_ring_keeps_published_event() {
        let state = state();
        let response = dispatch(
            Request {
                id: None,
                method: "event.publish".into(),
                params: json!({"kind": "ghost.trace", "data": {"route": "local"}}),
            },
            &state,
        );
        assert!(response.ok);
        assert_eq!(state.events.lock().unwrap().len(), 1);
    }

    #[test]
    fn resolution_rpc_closes_the_selected_objective() {
        let state = state();
        let start = dispatch(
            Request {
                id: Some("start".into()),
                method: "ghost.resolution.start".into(),
                params: json!({"id": "resolution-safe-workspace"}),
            },
            &state,
        );
        assert!(start.ok, "{:?}", start.error);
        assert_eq!(start.result["transition"]["status"], "awaiting-approval");

        let decide = dispatch(
            Request {
                id: Some("decide".into()),
                method: "ghost.resolution.decide".into(),
                params: json!({"id": "resolution-safe-workspace", "approved": true}),
            },
            &state,
        );
        assert!(decide.ok, "{:?}", decide.error);
        assert_eq!(decide.result["transition"]["status"], "resolved");
        assert_eq!(
            decide.result["resolutionLoop"]["resolutions"][0]["progress"],
            100
        );

        let reloaded = load_or_create(&state.resolution_path).unwrap();
        assert_eq!(reloaded.resolutions[0].status, "resolved");
        let parent = state.resolution_path.parent().unwrap();
        fs::remove_dir_all(parent).unwrap();
    }

    #[test]
    fn audit_rpc_accepts_real_input_and_writes_only_the_isolated_artifact() {
        let state = state();
        let input = "sudo rm -rf / — isto deve permanecer texto, não comando";
        let prepared = dispatch(
            Request {
                id: Some("prepare".into()),
                method: "ghost.audit.prepare".into(),
                params: json!({"input": input}),
            },
            &state,
        );
        assert!(prepared.ok, "{:?}", prepared.error);
        assert_eq!(prepared.result["transition"]["status"], "awaiting-approval");
        assert_eq!(
            prepared.result["transition"]["classification"]["risk"],
            "critical"
        );
        let id = prepared.result["transition"]["auditId"]
            .as_str()
            .unwrap()
            .to_string();

        let decided = dispatch(
            Request {
                id: Some("decide".into()),
                method: "ghost.audit.decide".into(),
                params: json!({"id": id, "approved": true}),
            },
            &state,
        );
        assert!(decided.ok, "{:?}", decided.error);
        assert_eq!(
            decided.result["transition"]["status"],
            "executed-awaiting-verification"
        );
        let artifact_path = PathBuf::from(
            decided.result["transition"]["artifactPath"]
                .as_str()
                .unwrap(),
        );
        let artifact = fs::read_to_string(&artifact_path).unwrap();
        assert!(artifact.contains(input));
        assert!(artifact.contains("ghost.audit.proof.write"));

        fs::remove_dir_all(state.audit_path.parent().unwrap()).unwrap();
    }
}
