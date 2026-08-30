use serde::{Deserialize, Serialize};
use serde_json::{json, Value};
use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::time::{SystemTime, UNIX_EPOCH};

pub const EVENT_LEDGER_SCHEMA: &str = "solos.daemon.event-ledger.v1";
pub const EVENT_SCHEMA: &str = "solos.daemon.event.v2";
pub const EVENT_LIMIT: usize = 2_048;

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct DaemonEvent {
    pub schema: String,
    pub event_id: String,
    pub sequence: u64,
    pub kind: String,
    pub timestamp: u64,
    pub data: Value,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct EventLedger {
    pub schema: String,
    pub updated_at: u64,
    pub next_sequence: u64,
    pub events: Vec<DaemonEvent>,
}

pub fn default_store_path() -> PathBuf {
    if let Some(path) = env::var_os("SOLOS_EVENT_STORE") {
        return PathBuf::from(path);
    }
    if let Some(path) = env::var_os("XDG_STATE_HOME") {
        return PathBuf::from(path).join("solos/ghost-events.json");
    }
    if let Some(path) = env::var_os("HOME") {
        return PathBuf::from(path).join(".local/state/solos/ghost-events.json");
    }
    env::temp_dir().join("solos/ghost-events.json")
}

pub fn empty_ledger() -> EventLedger {
    EventLedger {
        schema: EVENT_LEDGER_SCHEMA.into(),
        updated_at: now(),
        next_sequence: 1,
        events: Vec::new(),
    }
}

pub fn load_or_create(path: &Path) -> Result<EventLedger, String> {
    if !path.is_file() {
        let ledger = empty_ledger();
        save_atomic(path, &ledger)?;
        return Ok(ledger);
    }
    let payload = fs::read_to_string(path)
        .map_err(|error| format!("could not read event ledger {}: {error}", path.display()))?;
    let ledger: EventLedger = serde_json::from_str(&payload)
        .map_err(|error| format!("invalid event ledger {}: {error}", path.display()))?;
    if ledger.schema != EVENT_LEDGER_SCHEMA {
        return Err(format!(
            "unsupported event ledger schema: {}",
            ledger.schema
        ));
    }
    if ledger.events.len() > EVENT_LIMIT {
        return Err(format!(
            "event ledger exceeds {EVENT_LIMIT} retained events"
        ));
    }
    Ok(ledger)
}

pub fn append(ledger: &mut EventLedger, kind: &str, data: Value) -> Result<DaemonEvent, String> {
    validate_kind(kind)?;
    validate_local_data(&data)?;
    let timestamp = now();
    let sequence = ledger.next_sequence.max(1);
    let event = DaemonEvent {
        schema: EVENT_SCHEMA.into(),
        event_id: format!("evt-{timestamp}-{sequence}"),
        sequence,
        kind: kind.into(),
        timestamp,
        data,
    };
    ledger.next_sequence = sequence.saturating_add(1);
    ledger.updated_at = timestamp;
    ledger.events.push(event.clone());
    if ledger.events.len() > EVENT_LIMIT {
        let overflow = ledger.events.len() - EVENT_LIMIT;
        ledger.events.drain(0..overflow);
    }
    Ok(event)
}

pub fn list_after(ledger: &EventLedger, after_sequence: u64, limit: usize) -> Vec<DaemonEvent> {
    ledger
        .events
        .iter()
        .filter(|event| event.sequence > after_sequence)
        .take(limit.clamp(1, 256))
        .cloned()
        .collect()
}

pub fn export_brain_events(ledger: &EventLedger, after_sequence: u64, limit: usize) -> Vec<Value> {
    list_after(ledger, after_sequence, limit)
        .into_iter()
        .map(|event| {
            let (component, stage, status, severity, summary) = describe_for_brain(&event.kind);
            json!({
                "event_key": format!("solos-daemon:{}", event.event_id),
                "source": "solos_daemon",
                "component": component,
                "stage": stage,
                "algorithm_version": "solos-daemon-event-ledger-v1",
                "status": status,
                "severity": severity,
                "summary": summary,
                "metrics": { "sequence": event.sequence },
                "evidence": {
                    "event_id": event.event_id,
                    "event_kind": event.kind,
                    "event_schema": event.schema,
                    "occurred_at_unix": event.timestamp
                },
                "contains_personal_data": false,
                "occurred_at": event.timestamp
            })
        })
        .collect()
}

pub fn save_atomic(path: &Path, ledger: &EventLedger) -> Result<(), String> {
    if let Some(parent) = path.parent() {
        fs::create_dir_all(parent)
            .map_err(|error| format!("could not create event ledger directory: {error}"))?;
    }
    let temporary = path.with_extension(format!("json.tmp-{}", std::process::id()));
    let payload = serde_json::to_vec_pretty(ledger)
        .map_err(|error| format!("could not serialize event ledger: {error}"))?;
    fs::write(&temporary, payload)
        .map_err(|error| format!("could not write temporary event ledger: {error}"))?;
    fs::rename(&temporary, path)
        .map_err(|error| format!("could not commit event ledger: {error}"))?;
    Ok(())
}

fn validate_kind(kind: &str) -> Result<(), String> {
    let allowed = ["daemon.", "shell.", "ghost.", "pulso.", "wallet.", "apps."];
    if kind.len() > 120 || !allowed.iter().any(|prefix| kind.starts_with(prefix)) {
        return Err("event kind must use an allowed daemon.*, shell.*, ghost.*, pulso.*, wallet.*, or apps.* namespace".into());
    }
    Ok(())
}

fn validate_local_data(data: &Value) -> Result<(), String> {
    let size = serde_json::to_vec(data)
        .map_err(|error| format!("could not serialize event data: {error}"))?
        .len();
    if size > 64 * 1024 {
        return Err("event data exceeds 64 KiB".into());
    }
    Ok(())
}

fn describe_for_brain(
    kind: &str,
) -> (
    &'static str,
    &'static str,
    &'static str,
    &'static str,
    &'static str,
) {
    match kind {
        "daemon.started" => (
            "runtime-event-ledger",
            "sync",
            "started",
            "info",
            "SolOS Daemon iniciou com o ledger de evidências disponível.",
        ),
        "ghost.resolution.selected" => (
            "resolution-loop",
            "propose",
            "selected",
            "info",
            "Ghost selecionou um objetivo limitado para resolução.",
        ),
        "ghost.resolution.awaiting-approval" => (
            "resolution-loop",
            "review",
            "awaiting-approval",
            "info",
            "Ghost parou no limite de aprovação antes de agir.",
        ),
        "ghost.resolution.decided" => (
            "resolution-loop",
            "activate",
            "decided",
            "info",
            "Uma resolução Ghost recebeu decisão e preservou o resultado verificável.",
        ),
        "ghost.resolutions.reset" => (
            "resolution-loop",
            "rollback",
            "reset",
            "warning",
            "O demonstrador de resolução Ghost foi restaurado para a semente segura.",
        ),
        "ghost.audit.awaiting-approval" => (
            "audit-pilot",
            "observe",
            "awaiting-approval",
            "info",
            "Ghost classificou um input real como dado inerte e aguardou aprovação.",
        ),
        "ghost.audit.decided" => (
            "audit-pilot",
            "review",
            "decided",
            "info",
            "O piloto de auditoria recebeu uma decisão humana explícita.",
        ),
        "ghost.audit.verified" => (
            "audit-pilot",
            "evaluate",
            "verified",
            "info",
            "O verificador independente conferiu o recibo do piloto de auditoria.",
        ),
        _ if kind.starts_with("pulso.") => (
            "pulso-adapter",
            "observe",
            "recorded",
            "info",
            "O adaptador Pulso registrou uma evidência operacional agregada.",
        ),
        _ if kind.starts_with("wallet.") => (
            "wallet-adapter",
            "observe",
            "recorded",
            "info",
            "O adaptador Wallet registrou uma evidência operacional local.",
        ),
        _ if kind.starts_with("apps.") => (
            "apps-adapter",
            "observe",
            "recorded",
            "info",
            "O adaptador Apps registrou uma evidência operacional local.",
        ),
        _ if kind.starts_with("shell.") => (
            "shell-adapter",
            "observe",
            "recorded",
            "info",
            "A shell SolOS registrou uma evidência operacional local.",
        ),
        _ => (
            "ghost-runtime",
            "observe",
            "recorded",
            "info",
            "O Ghost registrou uma evidência operacional local.",
        ),
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

    fn temp_path(label: &str) -> PathBuf {
        env::temp_dir().join(format!(
            "solos-{label}-{}-{}.json",
            std::process::id(),
            now()
        ))
    }

    #[test]
    fn ledger_round_trip_preserves_sequence() {
        let path = temp_path("event-ledger-round-trip");
        let mut ledger = empty_ledger();
        let first = append(&mut ledger, "ghost.trace", json!({"route": "local"})).unwrap();
        save_atomic(&path, &ledger).unwrap();
        let mut reloaded = load_or_create(&path).unwrap();
        let second = append(&mut reloaded, "pulso.aggregate", json!({"posts": 2})).unwrap();
        assert_eq!(second.sequence, first.sequence + 1);
        fs::remove_file(path).unwrap();
    }

    #[test]
    fn brain_export_never_copies_local_payload() {
        let mut ledger = empty_ledger();
        append(
            &mut ledger,
            "ghost.audit.awaiting-approval",
            json!({"input": "private"}),
        )
        .unwrap();
        let export = export_brain_events(&ledger, 0, 10);
        let serialized = serde_json::to_string(&export).unwrap();
        assert!(!serialized.contains("private"));
        assert_eq!(export[0]["contains_personal_data"], false);
    }

    #[test]
    fn external_namespaces_fail_closed() {
        let mut ledger = empty_ledger();
        assert!(append(&mut ledger, "public.post", Value::Null).is_err());
    }
}
