use serde::Serialize;
use std::env;
use std::fs;
use std::path::Path;
use std::process::Command;
use std::time::Duration;

#[derive(Serialize)]
#[allow(non_snake_case)]
struct RuntimeSnapshot {
    sessionLabel: String,
    systemLabel: String,
    walletLabel: String,
    agentStatus: String,
    runtimeMode: String,
    runtimeSource: String,
    runtimeRole: String,
    mediationStatus: String,
    home: HomeState,
    ghost: GhostState,
    quickActions: Vec<QuickAction>,
    activityFeed: Vec<ActivityEntry>,
    approvals: Vec<ApprovalEntry>,
    apps: Vec<AppEntry>,
    hostRuntime: HostRuntime,
    systemStatus: SystemStatus,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct HomeState {
    summaryTitle: String,
    summarySubtitle: String,
    summaryBody: String,
    nextActionTitle: String,
    nextActionSubtitle: String,
    nextActionBody: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
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
#[allow(non_snake_case)]
struct ApprovalEntry {
    id: String,
    title: String,
    description: String,
    requestedBy: String,
    capability: String,
    scope: String,
    risk: String,
    status: String,
    createdAt: String,
}

#[derive(Serialize)]
struct AppEntry {
    name: String,
    subtitle: String,
    description: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct HostRuntime {
    os: String,
    kernel: String,
    initSystem: String,
    sessionType: String,
    desktopSession: String,
    shell: String,
    hostname: String,
    user: String,
    uptime: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct SystemStatus {
    online: bool,
    approvalsCount: usize,
    notificationsCount: usize,
    hostRuntimeSummary: String,
}

fn main() {
    let host = detect_host_runtime();
    let online = detect_online();
    let app_registry = build_app_registry();
    let approvals = build_approvals();
    let activity_feed = build_activity_feed(&host, online, app_registry.len());
    let quick_actions = build_quick_actions();
    let approvals_count = approvals.len();
    let notifications_count = activity_feed.len();

    let snapshot = RuntimeSnapshot {
        sessionLabel: format!("{} · SolOS operating layer active", host.user),
        systemLabel: format!(
            "Linux base system attached · {} · {} · {}",
            host.initSystem, host.sessionType, if online { "online" } else { "offline" }
        ),
        walletLabel: "Wallet bridge pending · ownership surface visible".into(),
        agentStatus: "Ghost active · runtime intermediary attached".into(),
        runtimeMode: "runtime-intermediary".into(),
        runtimeSource: "Rust runtime-core mediates between Linux host services and the SolOS operating layer.".into(),
        runtimeRole: "Intermediary layer between Linux base system and SolOS operating layer".into(),
        mediationStatus: "Host facts detected and normalized into SolOS-facing runtime state".into(),
        home: HomeState {
            summaryTitle: "SolOS v1.0 runs above a runtime intermediary".into(),
            summarySubtitle: "Linux is the base system, the runtime mediates, and SolOS delivers the operating layer.".into(),
            summaryBody: format!(
                "This build treats runtime-core as the middle layer between Linux and SolOS. It reads host state from {}, kernel {}, hostname {}, user {}, uptime {}, and current session plumbing, then exposes that state as stable runtime context for the shell.",
                host.os, host.kernel, host.hostname, host.user, host.uptime
            ),
            nextActionTitle: "Next useful move".into(),
            nextActionSubtitle: "Promote mediation into real runtime services".into(),
            nextActionBody: "Wire approvals, app launch, wallet bridges, and agent actions through runtime-mediated services instead of leaving the shell coupled to static fixtures.".into(),
        },
        ghost: GhostState {
            presenceLabel: "Ghost present in shell".into(),
            modeLabel: "runtime-mediated · approval-aware · host-integrated".into(),
            thesisLabel: "SolOS should sit above a runtime intermediary that translates Linux host reality into stable operating-layer semantics.".into(),
        },
        quickActions: quick_actions,
        activityFeed: activity_feed,
        approvals,
        apps: app_registry,
        systemStatus: SystemStatus {
            online,
            approvalsCount: approvals_count,
            notificationsCount: notifications_count,
            hostRuntimeSummary: format!(
                "{} · {} · {} · uptime {}",
                host.hostname, host.user, host.sessionType, host.uptime
            ),
        },
        hostRuntime: host,
    };

    println!("{}", serde_json::to_string_pretty(&snapshot).unwrap());
}

fn detect_host_runtime() -> HostRuntime {
    HostRuntime {
        os: read_os_pretty_name().unwrap_or_else(|| "Linux host".into()),
        kernel: run("uname", &["-r"]).unwrap_or_else(|| "unknown-kernel".into()),
        initSystem: detect_init_system(),
        sessionType: env::var("XDG_SESSION_TYPE").unwrap_or_else(|_| "unknown-session".into()),
        desktopSession: env::var("XDG_CURRENT_DESKTOP")
            .or_else(|_| env::var("DESKTOP_SESSION"))
            .unwrap_or_else(|_| "unknown-desktop".into()),
        shell: env::var("SHELL").unwrap_or_else(|_| "unknown-shell".into()),
        hostname: run("hostname", &[]).unwrap_or_else(|| "unknown-host".into()),
        user: env::var("USER").unwrap_or_else(|_| "unknown-user".into()),
        uptime: detect_uptime(),
    }
}

fn detect_online() -> bool {
    ["/sys/class/net/wlan0/operstate", "/sys/class/net/eth0/operstate"]
        .iter()
        .any(|path| fs::read_to_string(path).map(|v| v.trim() == "up").unwrap_or(false))
        || run("ip", &["route", "show", "default"]).is_some()
}

fn detect_uptime() -> String {
    let raw = fs::read_to_string("/proc/uptime")
        .ok()
        .and_then(|content| content.split_whitespace().next()?.parse::<f64>().ok());

    match raw {
        Some(seconds) => format_duration(Duration::from_secs_f64(seconds)),
        None => "unknown-uptime".into(),
    }
}

fn format_duration(duration: Duration) -> String {
    let total_seconds = duration.as_secs();
    let days = total_seconds / 86_400;
    let hours = (total_seconds % 86_400) / 3_600;
    let minutes = (total_seconds % 3_600) / 60;

    if days > 0 {
        format!("{}d {}h {}m", days, hours, minutes)
    } else if hours > 0 {
        format!("{}h {}m", hours, minutes)
    } else {
        format!("{}m", minutes)
    }
}

fn build_quick_actions() -> Vec<QuickAction> {
    vec![
        QuickAction {
            title: "Inspect mediated host services".into(),
            subtitle: "Runtime as intermediary".into(),
            description: "Read systemd, session, network, and process state through the runtime layer instead of exposing raw host details everywhere in the shell.".into(),
        },
        QuickAction {
            title: "Bind approvals to mediated commands".into(),
            subtitle: "Controlled execution path".into(),
            description: "Move approvals from static queue items toward runtime-mediated Linux actions with explicit scope and rollback expectations.".into(),
        },
        QuickAction {
            title: "Assemble ISO demo".into(),
            subtitle: "Package all three layers".into(),
            description: "Boot Linux first, initialize the runtime intermediary, then launch SolOS as the visible operating layer with documented image assembly steps.".into(),
        },
    ]
}

fn build_activity_feed(host: &HostRuntime, online: bool, app_count: usize) -> Vec<ActivityEntry> {
    vec![
        ActivityEntry {
            title: "Linux base system detected".into(),
            detail: format!(
                "Runtime intermediary attached to {} on {} with {} as init, {} as the active session type, and uptime {}.",
                host.os, host.hostname, host.initSystem, host.sessionType, host.uptime
            ),
            status: "active".into(),
        },
        ActivityEntry {
            title: "Network state mediated".into(),
            detail: format!(
                "Runtime reports the host as {} and can now expose online/offline state as a stable SolOS contract.",
                if online { "online" } else { "offline" }
            ),
            status: if online { "active" } else { "warning" }.into(),
        },
        ActivityEntry {
            title: "App registry surfaced".into(),
            detail: format!(
                "{} operating-layer modules are now described as registry-backed entries instead of loose shell copy.",
                app_count
            ),
            status: "active".into(),
        },
    ]
}

fn build_approvals() -> Vec<ApprovalEntry> {
    vec![
        ApprovalEntry {
            id: "approval-host-services".into(),
            title: "Grant controlled access to mediated system services".into(),
            description: "Allow runtime-core to expose selected session, launcher, and service information through a bounded SolOS contract.".into(),
            requestedBy: "runtime-core".into(),
            capability: "host.service.read".into(),
            scope: "runtime intermediary -> systemd / desktop session / launchers".into(),
            risk: "medium".into(),
            status: "pending".into(),
            createdAt: "runtime-bootstrap".into(),
        },
        ApprovalEntry {
            id: "approval-wallet-broker".into(),
            title: "Connect signing and wallet actions to explicit mediated brokers".into(),
            description: "Prepare a wallet broker boundary so signing remains visible, scoped, and never hidden behind generic agent behavior.".into(),
            requestedBy: "ghost-console".into(),
            capability: "wallet.sign.request".into(),
            scope: "wallet bridge / secure approvals / runtime mediation".into(),
            risk: "high".into(),
            status: "pending".into(),
            createdAt: "runtime-bootstrap".into(),
        },
    ]
}

fn build_app_registry() -> Vec<AppEntry> {
    vec![
        AppEntry {
            name: "Workspace".into(),
            subtitle: "Operating layer context".into(),
            description: "Coordinates user tasks, notes, and live environment state above the runtime intermediary.".into(),
        },
        AppEntry {
            name: "Approval Lane".into(),
            subtitle: "Policy boundary".into(),
            description: "Surfaces runtime-mediated host actions that require explicit consent before execution.".into(),
        },
        AppEntry {
            name: "Wallet Hub".into(),
            subtitle: "Ownership surface".into(),
            description: "Keeps identity, balances, and signing visible through explicit runtime-mediated flows.".into(),
        },
        AppEntry {
            name: "Ghost Console".into(),
            subtitle: "Native agent presence".into(),
            description: "Turns runtime-mediated host state into agent-readable, approval-aware orchestration inside SolOS.".into(),
        },
    ]
}

fn read_os_pretty_name() -> Option<String> {
    let content = std::fs::read_to_string("/etc/os-release").ok()?;
    for line in content.lines() {
        if let Some(value) = line.strip_prefix("PRETTY_NAME=") {
            return Some(value.trim_matches('"').to_string());
        }
    }
    None
}

fn detect_init_system() -> String {
    if let Ok(target) = std::fs::read_link("/proc/1/exe") {
        if let Some(name) = Path::new(&target).file_name().and_then(|n| n.to_str()) {
            return name.to_string();
        }
    }

    run("ps", &["-p", "1", "-o", "comm="]).unwrap_or_else(|| "unknown-init".into())
}

fn run(command: &str, args: &[&str]) -> Option<String> {
    let output = Command::new(command).args(args).output().ok()?;
    if !output.status.success() {
        return None;
    }

    let text = String::from_utf8(output.stdout).ok()?;
    let trimmed = text.trim();
    if trimmed.is_empty() {
        None
    } else {
        Some(trimmed.to_string())
    }
}
