use serde::Serialize;

#[derive(Serialize)]
struct RuntimeSnapshot {
    sessionLabel: String,
    systemLabel: String,
    walletLabel: String,
    agentStatus: String,
    home: HomeState,
    ghost: GhostState,
    quickActions: Vec<QuickAction>,
    activityFeed: Vec<ActivityEntry>,
    approvals: Vec<ApprovalEntry>,
    apps: Vec<AppEntry>,
}

#[derive(Serialize)]
struct HomeState {
    summaryTitle: String,
    summarySubtitle: String,
    summaryBody: String,
    nextActionTitle: String,
    nextActionSubtitle: String,
    nextActionBody: String,
}

#[derive(Serialize)]
struct GhostState {
    presenceLabel: String,
    modeLabel: String,
    thesisLabel: String,
}

#[derive(Serialize)]
struct QuickAction {
    title: String,
    subtitle: String,
    description: String,
}

#[derive(Serialize)]
struct ActivityEntry {
    title: String,
    detail: String,
    status: String,
}

#[derive(Serialize)]
struct ApprovalEntry {
    title: String,
    scope: String,
    risk: String,
}

#[derive(Serialize)]
struct AppEntry {
    name: String,
    subtitle: String,
    description: String,
}

fn main() {
    let snapshot = RuntimeSnapshot {
        sessionLabel: "Luiz · SolOS Environment Active".into(),
        systemLabel: "Online · v0.3-live-refresh · Rust generator active".into(),
        walletLabel: "Solana · 9xLu...Ghost · 12.84 SOL · 248.00 USDC".into(),
        agentStatus: "Ghost active · runtime generated snapshot loaded".into(),
        home: HomeState {
            summaryTitle: "Operational center of gravity".into(),
            summarySubtitle: "Agency, approvals, identity, and apps in one field".into(),
            summaryBody: "Rust now generates a runtime snapshot that the native shell can re-read while it is running. That gives SolOS a visible pulse instead of a static conceptual mock.".into(),
            nextActionTitle: "Next useful move".into(),
            nextActionSubtitle: "Make runtime generation the source of visible movement".into(),
            nextActionBody: "Regenerate the snapshot and the shell should visibly shift. This keeps architecture work honest by tying backend progress to perceivable product change.".into(),
        },
        ghost: GhostState {
            presenceLabel: "Ghost present in shell".into(),
            modeLabel: "Rust-generated · approval-aware · live-refreshing".into(),
            thesisLabel: "Native agent presence should read from explicit runtime state, not scattered UI literals.".into(),
        },
        quickActions: vec![
            QuickAction {
                title: "Regenerate runtime".into(),
                subtitle: "Useful visible change".into(),
                description: "Re-run the Rust generator and watch the shell refresh into a new state without a deep rebuild.".into(),
            },
            QuickAction {
                title: "Audit approval lane".into(),
                subtitle: "System trust surface".into(),
                description: "Turn approvals into a durable operating-system primitive instead of ad hoc prompts.".into(),
            },
            QuickAction {
                title: "Expand live app registry".into(),
                subtitle: "Runtime-fed modules".into(),
                description: "Let the Apps surface become a live registry driven by the runtime contract.".into(),
            },
        ],
        activityFeed: vec![
            ActivityEntry {
                title: "Rust generator active".into(),
                detail: "The runtime core can now emit the snapshot contract consumed by the native shell.".into(),
                status: "active".into(),
            },
            ActivityEntry {
                title: "Shell refresh loop active".into(),
                detail: "The shell can manually or automatically refresh runtime-backed state while running.".into(),
                status: "active".into(),
            },
            ActivityEntry {
                title: "Visible product movement achieved".into(),
                detail: "The architecture pass now produces perceivable changes in Home, Agent, and Apps screens.".into(),
                status: "active".into(),
            },
        ],
        approvals: vec![
            ApprovalEntry {
                title: "Promote generated snapshot to default dev loop".into(),
                scope: "runtime-core -> shell-native".into(),
                risk: "medium".into(),
            },
            ApprovalEntry {
                title: "Connect approval items to executable actions".into(),
                scope: "agent and wallet surfaces".into(),
                risk: "high".into(),
            },
        ],
        apps: vec![
            AppEntry {
                name: "Workspace".into(),
                subtitle: "Core environment".into(),
                description: "Tasks, notes, and active operational context for the system and the user.".into(),
            },
            AppEntry {
                name: "Approval Lane".into(),
                subtitle: "Trust and action boundary".into(),
                description: "A dedicated lane for approvals, scope, and explicit consent.".into(),
            },
            AppEntry {
                name: "Wallet Hub".into(),
                subtitle: "Ownership surface".into(),
                description: "Balances, identity, signatures, and assets in one visible operating surface.".into(),
            },
            AppEntry {
                name: "Ghost Console".into(),
                subtitle: "Native agent presence".into(),
                description: "A runtime-backed agent surface for context, tasks, and orchestration.".into(),
            },
        ],
    };

    println!("{}", serde_json::to_string_pretty(&snapshot).unwrap());
}
