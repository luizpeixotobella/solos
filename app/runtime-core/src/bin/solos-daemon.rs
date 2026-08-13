use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use std::collections::VecDeque;
use std::env;
use std::fs;
use std::io::{BufRead, BufReader, Write};
use std::os::unix::fs::PermissionsExt;
use std::os::unix::net::{UnixListener, UnixStream};
use std::path::{Path, PathBuf};
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

    prepare_socket(&socket_path)?;
    let listener = UnixListener::bind(&socket_path)?;
    fs::set_permissions(&socket_path, fs::Permissions::from_mode(0o600))?;

    let state = DaemonState {
        started_at: now(),
        snapshot_path,
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
        })),
        "snapshot.get" => read_snapshot(&state.snapshot_path),
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

fn read_snapshot(path: &Path) -> Result<Value, String> {
    let content = fs::read_to_string(path)
        .map_err(|error| format!("snapshot unavailable at {}: {error}", path.display()))?;
    serde_json::from_str(&content).map_err(|error| format!("invalid runtime snapshot: {error}"))
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
        DaemonState {
            started_at: now(),
            snapshot_path: PathBuf::from("/path/that/does/not/exist"),
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
}
