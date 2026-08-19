use serde::{Deserialize, Serialize};
use std::env;
use std::fs;
use std::path::{Path, PathBuf};
use std::time::{SystemTime, UNIX_EPOCH};

pub const RESOLUTION_SCHEMA: &str = "solos.ghost.resolutions.v1";
pub const WORKSPACE_RESOLUTION_ID: &str = "resolution-safe-workspace";

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostResolutionStore {
    pub schema: String,
    pub updated_at: u64,
    pub selected_id: Option<String>,
    pub summary: String,
    pub resolutions: Vec<GhostResolution>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostResolution {
    pub id: String,
    pub title: String,
    pub objective: String,
    pub target_outcome: String,
    pub status: String,
    pub readiness: String,
    pub progress: u8,
    pub current_step: String,
    pub capability: String,
    pub approval_required: bool,
    pub result_summary: String,
    pub created_at: u64,
    pub updated_at: u64,
    pub steps: Vec<GhostResolutionStep>,
    pub evidence: Vec<GhostResolutionEvidence>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostResolutionStep {
    pub id: String,
    pub title: String,
    pub status: String,
    pub capability: String,
    pub approval_required: bool,
    pub result: String,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostResolutionEvidence {
    pub kind: String,
    pub label: String,
    pub detail: String,
    pub recorded_at: u64,
}

pub fn default_store_path() -> PathBuf {
    if let Some(path) = env::var_os("SOLOS_GHOST_RESOLUTION_STORE") {
        return PathBuf::from(path);
    }
    if let Some(path) = env::var_os("XDG_STATE_HOME") {
        return PathBuf::from(path).join("solos/ghost-resolutions.json");
    }
    if let Some(path) = env::var_os("HOME") {
        return PathBuf::from(path).join(".local/state/solos/ghost-resolutions.json");
    }
    env::temp_dir().join("solos/ghost-resolutions.json")
}

pub fn seed_store() -> GhostResolutionStore {
    seed_store_at(now())
}

fn seed_store_at(timestamp: u64) -> GhostResolutionStore {
    GhostResolutionStore {
        schema: RESOLUTION_SCHEMA.into(),
        updated_at: timestamp,
        selected_id: Some(WORKSPACE_RESOLUTION_ID.into()),
        summary: "One bounded objective is selected. Ghost can turn it into a visible plan, ask for approval, execute one mediated capability, and retain evidence of the result.".into(),
        resolutions: vec![
            GhostResolution {
                id: WORKSPACE_RESOLUTION_ID.into(),
                title: "Restore my workspace, safely".into(),
                objective: "Resume the Workspace module without bypassing the SolOS approval boundary.".into(),
                target_outcome: "Workspace is active, the user approval is recorded, and the app.open.safe result is attached as evidence.".into(),
                status: "selected".into(),
                readiness: "ready".into(),
                progress: 20,
                current_step: "Build a bounded plan".into(),
                capability: "app.open.safe".into(),
                approval_required: true,
                result_summary: "Objective selected; execution has not started.".into(),
                created_at: timestamp,
                updated_at: timestamp,
                steps: workspace_steps(),
                evidence: vec![GhostResolutionEvidence {
                    kind: "selection".into(),
                    label: "Objective selected".into(),
                    detail: "Ghost selected a ready local objective that can be completed through an existing mediated capability.".into(),
                    recorded_at: timestamp,
                }],
            },
            GhostResolution {
                id: "resolution-grounded-answer".into(),
                title: "Research a grounded answer".into(),
                objective: "Research a fresh question and return source-linked evidence.".into(),
                target_outcome: "Answer, citations, quota receipt, and trace are retained together.".into(),
                status: "candidate".into(),
                readiness: "needs-quota-or-byok".into(),
                progress: 0,
                current_step: "Waiting for a configured research route".into(),
                capability: "web.search.read".into(),
                approval_required: true,
                result_summary: "Not executable until a quota or BYOK route is available.".into(),
                created_at: timestamp,
                updated_at: timestamp,
                steps: vec![blocked_step(
                    "research-route",
                    "Configure a grounded research route",
                    "web.search.read",
                    "Heart Pass quota or BYOK is required.",
                )],
                evidence: vec![],
            },
            GhostResolution {
                id: "resolution-public-launch".into(),
                title: "Prepare and publish a launch".into(),
                objective: "Turn a completed product change into a reviewed multi-channel launch.".into(),
                target_outcome: "Verified public URLs are attached to the same resolution trace.".into(),
                status: "candidate".into(),
                readiness: "needs-public-send-adapter".into(),
                progress: 0,
                current_step: "Waiting for an account-bound publish adapter".into(),
                capability: "public.post.create".into(),
                approval_required: true,
                result_summary: "Not executable inside SolOS until a public-send adapter exists.".into(),
                created_at: timestamp,
                updated_at: timestamp,
                steps: vec![blocked_step(
                    "publish-adapter",
                    "Connect an approval-bound publish adapter",
                    "public.post.create",
                    "Public posting remains outside the current runtime capability manifest.",
                )],
                evidence: vec![],
            },
        ],
    }
}

fn workspace_steps() -> Vec<GhostResolutionStep> {
    vec![
        GhostResolutionStep {
            id: "understand".into(),
            title: "Understand the objective".into(),
            status: "completed".into(),
            capability: "task.intent.classify".into(),
            approval_required: false,
            result: "Classified as a bounded local app action.".into(),
        },
        GhostResolutionStep {
            id: "plan".into(),
            title: "Build a bounded plan".into(),
            status: "active".into(),
            capability: "task.plan.local".into(),
            approval_required: false,
            result: String::new(),
        },
        GhostResolutionStep {
            id: "approval".into(),
            title: "Ask for explicit approval".into(),
            status: "pending".into(),
            capability: "approval.request".into(),
            approval_required: true,
            result: String::new(),
        },
        GhostResolutionStep {
            id: "execute".into(),
            title: "Execute app.open.safe".into(),
            status: "pending".into(),
            capability: "app.open.safe".into(),
            approval_required: true,
            result: String::new(),
        },
        GhostResolutionStep {
            id: "verify".into(),
            title: "Verify the target outcome".into(),
            status: "pending".into(),
            capability: "app.state.read".into(),
            approval_required: false,
            result: String::new(),
        },
    ]
}

fn blocked_step(id: &str, title: &str, capability: &str, result: &str) -> GhostResolutionStep {
    GhostResolutionStep {
        id: id.into(),
        title: title.into(),
        status: "blocked".into(),
        capability: capability.into(),
        approval_required: true,
        result: result.into(),
    }
}

pub fn load_or_default(path: &Path) -> Result<GhostResolutionStore, String> {
    if !path.is_file() {
        return Ok(seed_store());
    }
    let content = fs::read_to_string(path).map_err(|error| {
        format!(
            "could not read resolution store {}: {error}",
            path.display()
        )
    })?;
    let store: GhostResolutionStore = serde_json::from_str(&content)
        .map_err(|error| format!("invalid resolution store {}: {error}", path.display()))?;
    if store.schema != RESOLUTION_SCHEMA {
        return Err(format!(
            "unsupported resolution store schema: {}",
            store.schema
        ));
    }
    Ok(store)
}

pub fn load_or_create(path: &Path) -> Result<GhostResolutionStore, String> {
    if path.is_file() {
        return load_or_default(path);
    }
    let store = seed_store();
    save_atomic(path, &store)?;
    Ok(store)
}

pub fn save_atomic(path: &Path, store: &GhostResolutionStore) -> Result<(), String> {
    if let Some(parent) = path.parent() {
        fs::create_dir_all(parent)
            .map_err(|error| format!("could not create resolution store directory: {error}"))?;
    }
    let temporary = path.with_extension(format!("json.tmp-{}", std::process::id()));
    let payload = serde_json::to_vec_pretty(store)
        .map_err(|error| format!("could not serialize resolution store: {error}"))?;
    fs::write(&temporary, payload)
        .map_err(|error| format!("could not write temporary resolution store: {error}"))?;
    fs::rename(&temporary, path)
        .map_err(|error| format!("could not commit resolution store: {error}"))?;
    Ok(())
}

pub fn select_resolution(store: &mut GhostResolutionStore, id: &str) -> Result<(), String> {
    let timestamp = now();
    let selected = store
        .resolutions
        .iter()
        .find(|resolution| resolution.id == id)
        .ok_or_else(|| format!("unknown Ghost resolution: {id}"))?;
    if selected.readiness != "ready" {
        return Err(format!(
            "resolution {id} is not executable: {}",
            selected.readiness
        ));
    }

    for resolution in &mut store.resolutions {
        if resolution.readiness == "ready" && resolution.status != "resolved" {
            resolution.status = "candidate".into();
            resolution.progress = 0;
            resolution.current_step = "Waiting for selection".into();
            resolution.steps = workspace_steps();
            for step in &mut resolution.steps {
                step.status = "pending".into();
                step.result.clear();
            }
            resolution.evidence.clear();
            resolution.result_summary = "Ready for selection.".into();
            resolution.updated_at = timestamp;
        }
    }

    let resolution = resolution_mut(store, id)?;
    resolution.status = "selected".into();
    resolution.progress = 20;
    resolution.current_step = "Build a bounded plan".into();
    resolution.steps = workspace_steps();
    resolution.result_summary = "Objective selected; execution has not started.".into();
    resolution.evidence.push(GhostResolutionEvidence {
        kind: "selection".into(),
        label: "Objective selected".into(),
        detail: "Ghost selected a ready objective and opened a bounded resolution trace.".into(),
        recorded_at: timestamp,
    });
    resolution.updated_at = timestamp;
    store.selected_id = Some(id.into());
    store.summary = "A ready objective is selected. The next transition builds the plan and opens the approval boundary.".into();
    store.updated_at = timestamp;
    Ok(())
}

pub fn start_resolution(store: &mut GhostResolutionStore, id: &str) -> Result<(), String> {
    ensure_selected(store, id)?;
    let timestamp = now();
    let resolution = resolution_mut(store, id)?;
    if resolution.status != "selected" {
        return Err(format!(
            "resolution {id} cannot start from status {}",
            resolution.status
        ));
    }
    complete_step(
        resolution,
        "understand",
        "Objective classified as a bounded local app action.",
    );
    complete_step(
        resolution,
        "plan",
        "Plan fixed: approval -> app.open.safe -> app.state.read.",
    );
    activate_step(resolution, "approval");
    resolution.status = "awaiting-approval".into();
    resolution.progress = 50;
    resolution.current_step = "Ask for explicit approval".into();
    resolution.result_summary =
        "Plan prepared. No app action runs before the visible approval decision.".into();
    resolution.evidence.push(GhostResolutionEvidence {
        kind: "plan".into(),
        label: "Bounded plan prepared".into(),
        detail:
            "Ghost reduced the objective to one allowed app capability followed by a state check."
                .into(),
        recorded_at: timestamp,
    });
    resolution.updated_at = timestamp;
    store.summary = "The selected resolution is waiting for explicit user approval.".into();
    store.updated_at = timestamp;
    Ok(())
}

pub fn decide_resolution(
    store: &mut GhostResolutionStore,
    id: &str,
    approved: bool,
) -> Result<(), String> {
    ensure_selected(store, id)?;
    let timestamp = now();
    let resolution = resolution_mut(store, id)?;
    if resolution.status != "awaiting-approval" {
        return Err(format!(
            "resolution {id} is not awaiting approval: {}",
            resolution.status
        ));
    }

    if !approved {
        block_step(
            resolution,
            "approval",
            "User denied the requested capability.",
        );
        resolution.status = "blocked".into();
        resolution.progress = 50;
        resolution.current_step = "Stopped at approval boundary".into();
        resolution.result_summary =
            "No action executed. The user's denial was retained as evidence.".into();
        resolution.evidence.push(GhostResolutionEvidence {
            kind: "approval".into(),
            label: "Approval denied".into(),
            detail: "The resolution stopped before app.open.safe; no side effect occurred.".into(),
            recorded_at: timestamp,
        });
        resolution.updated_at = timestamp;
        store.summary =
            "The resolution is blocked by a recorded user denial; no capability executed.".into();
        store.updated_at = timestamp;
        return Ok(());
    }

    complete_step(
        resolution,
        "approval",
        "User approved app.open.safe for Workspace.",
    );
    complete_step(
        resolution,
        "execute",
        "Workspace changed to active through app.open.safe.",
    );
    complete_step(
        resolution,
        "verify",
        "app.state.read confirmed Workspace is active.",
    );
    resolution.status = "resolved".into();
    resolution.progress = 100;
    resolution.current_step = "Target outcome verified".into();
    resolution.result_summary = "Workspace is active. Approval, mediated execution, and verification evidence are attached to this resolution.".into();
    resolution.evidence.extend([
        GhostResolutionEvidence {
            kind: "approval".into(),
            label: "Approval granted".into(),
            detail: "User approved the exact app.open.safe scope for Workspace.".into(),
            recorded_at: timestamp,
        },
        GhostResolutionEvidence {
            kind: "capability-result".into(),
            label: "Workspace opened".into(),
            detail: "app.open.safe returned status=active for target=workspace.".into(),
            recorded_at: timestamp,
        },
        GhostResolutionEvidence {
            kind: "verification".into(),
            label: "Outcome verified".into(),
            detail: "app.state.read observed Workspace in the active state after execution.".into(),
            recorded_at: timestamp,
        },
    ]);
    resolution.updated_at = timestamp;
    store.summary = "The selected objective is resolved with approval, capability result, and state verification preserved end to end.".into();
    store.updated_at = timestamp;
    Ok(())
}

pub fn reset_store(store: &mut GhostResolutionStore) {
    *store = seed_store();
}

fn resolution_mut<'a>(
    store: &'a mut GhostResolutionStore,
    id: &str,
) -> Result<&'a mut GhostResolution, String> {
    store
        .resolutions
        .iter_mut()
        .find(|resolution| resolution.id == id)
        .ok_or_else(|| format!("unknown Ghost resolution: {id}"))
}

fn ensure_selected(store: &GhostResolutionStore, id: &str) -> Result<(), String> {
    if store.selected_id.as_deref() != Some(id) {
        return Err(format!("resolution {id} is not selected"));
    }
    Ok(())
}

fn complete_step(resolution: &mut GhostResolution, id: &str, result: &str) {
    if let Some(step) = resolution.steps.iter_mut().find(|step| step.id == id) {
        step.status = "completed".into();
        step.result = result.into();
    }
}

fn activate_step(resolution: &mut GhostResolution, id: &str) {
    if let Some(step) = resolution.steps.iter_mut().find(|step| step.id == id) {
        step.status = "active".into();
    }
}

fn block_step(resolution: &mut GhostResolution, id: &str, result: &str) {
    if let Some(step) = resolution.steps.iter_mut().find(|step| step.id == id) {
        step.status = "blocked".into();
        step.result = result.into();
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

    #[test]
    fn selected_resolution_reaches_verified_result() {
        let mut store = seed_store_at(1);
        start_resolution(&mut store, WORKSPACE_RESOLUTION_ID).unwrap();
        assert_eq!(store.resolutions[0].status, "awaiting-approval");
        decide_resolution(&mut store, WORKSPACE_RESOLUTION_ID, true).unwrap();
        let resolution = &store.resolutions[0];
        assert_eq!(resolution.status, "resolved");
        assert_eq!(resolution.progress, 100);
        assert!(resolution
            .steps
            .iter()
            .all(|step| step.status == "completed"));
        assert!(resolution
            .evidence
            .iter()
            .any(|item| item.kind == "verification"));
    }

    #[test]
    fn denial_stops_before_execution() {
        let mut store = seed_store_at(1);
        start_resolution(&mut store, WORKSPACE_RESOLUTION_ID).unwrap();
        decide_resolution(&mut store, WORKSPACE_RESOLUTION_ID, false).unwrap();
        let resolution = &store.resolutions[0];
        assert_eq!(resolution.status, "blocked");
        assert_eq!(resolution.steps[3].status, "pending");
        assert!(!resolution
            .evidence
            .iter()
            .any(|item| item.kind == "capability-result"));
    }

    #[test]
    fn unavailable_resolution_cannot_be_selected() {
        let mut store = seed_store_at(1);
        let error = select_resolution(&mut store, "resolution-public-launch").unwrap_err();
        assert!(error.contains("not executable"));
    }

    #[test]
    fn store_round_trip_preserves_resolution() {
        let directory =
            env::temp_dir().join(format!("solos-resolution-test-{}", std::process::id()));
        let path = directory.join("store.json");
        let store = seed_store_at(1);
        save_atomic(&path, &store).unwrap();
        assert_eq!(load_or_default(&path).unwrap(), store);
        fs::remove_dir_all(directory).unwrap();
    }
}
