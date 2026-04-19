use serde::Serialize;
use std::env;
use std::path::Path;
use std::process::Command;

#[derive(Serialize)]
struct RuntimeSnapshot {
    sessionLabel: String,
    systemLabel: String,
    walletLabel: String,
    agentStatus: String,
    runtimeMode: String,
    runtimeSource: String,
    home: HomeState,
    ghost: GhostState,
    quickActions: Vec<QuickAction>,
    activityFeed: Vec<ActivityEntry>,
    approvals: Vec<ApprovalEntry>,
    apps: Vec<AppEntry>,
    hostRuntime: HostRuntime,
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

#[derive(Serialize)]
struct HostRuntime {
    os: String,
    kernel: String,
    initSystem: String,
    sessionType: String,
    desktopSession: String,
    shell: String,
    hostname: String,
    user: String,
}

fn main() {
    let host = detect_host_runtime();

    let snapshot = RuntimeSnapshot {
        sessionLabel: format!("{} · SolOS operating layer active", host.user),
        systemLabel: format!(
            "Linux runtime active · {} · {}",
            host.initSystem, host.sessionType
        ),
        walletLabel: "Wallet bridge pending · ownership surface visible".into(),
        agentStatus: "Ghost active · Linux host runtime attached".into(),
        runtimeMode: "host-runtime".into(),
        runtimeSource: "SolOS runs as an operating layer on top of Linux. Rust is only the adapter that reads and organizes host runtime state.".into(),
        home: HomeState {
            summaryTitle: "SolOS v1.0 operates on the Linux host".into(),
            summarySubtitle: "The runtime belongs to Linux. SolOS composes experience, orchestration, and policy above it.".into(),
            summaryBody: format!(
                "This build no longer presents Rust as a self-contained runtime. SolOS reads the existing Linux runtime, session, and process environment, then turns that host state into shell-visible orchestration. Host: {} on kernel {}.",
                host.os, host.kernel
            ),
            nextActionTitle: "Next useful move".into(),
            nextActionSubtitle: "Promote Linux-backed services into first-class SolOS capabilities".into(),
            nextActionBody: "Wire approvals, app launch, wallet bridges, and agent actions into real Linux services instead of extending static shell copy.".into(),
        },
        ghost: GhostState {
            presenceLabel: "Ghost present in shell".into(),
            modeLabel: "Linux-backed · approval-aware · host-integrated".into(),
            thesisLabel: "SolOS should not fake a kernel or invent a parallel runtime when Linux already provides the execution substrate.".into(),
        },
        quickActions: vec![
            QuickAction {
                title: "Inspect host services".into(),
                subtitle: "Use Linux as substrate".into(),
                description: "Read systemd, session, network, and process state from the host instead of recreating those primitives inside SolOS.".into(),
            },
            QuickAction {
                title: "Bind approvals to real commands".into(),
                subtitle: "Stop staging fake actions".into(),
                description: "Move approvals from static queue items toward mediated Linux actions with explicit scope and rollback expectations.".into(),
            },
            QuickAction {
                title: "Assemble ISO demo".into(),
                subtitle: "Package the operating layer".into(),
                description: "Boot Linux first, then launch SolOS as the default shell experience with documented image assembly steps.".into(),
            },
        ],
        activityFeed: vec![
            ActivityEntry {
                title: "Host runtime detected".into(),
                detail: format!(
                    "SolOS is attached to {} with {} as init and {} as the active session type.",
                    host.os, host.initSystem, host.sessionType
                ),
                status: "active".into(),
            },
            ActivityEntry {
                title: "Runtime ownership corrected".into(),
                detail: "Rust now acts as a runtime adapter layer, not as a pretend operating-system runtime replacing Linux.".into(),
                status: "active".into(),
            },
            ActivityEntry {
                title: "ISO demo path ready".into(),
                detail: "The appliance folder defines how to package SolOS v1.0 as a Linux image instead of describing an abstract future distro.".into(),
                status: "active".into(),
            },
        ],
        approvals: vec![
            ApprovalEntry {
                title: "Grant controlled access to system services".into(),
                scope: "SolOS bridge -> systemd / desktop session / launchers".into(),
                risk: "medium".into(),
            },
            ApprovalEntry {
                title: "Connect signing and wallet actions to explicit Linux-backed brokers".into(),
                scope: "wallet bridge / secure approvals".into(),
                risk: "high".into(),
            },
        ],
        apps: vec![
            AppEntry {
                name: "Workspace".into(),
                subtitle: "Operating layer context".into(),
                description: "Coordinates user tasks, notes, and live environment state on top of the Linux host.".into(),
            },
            AppEntry {
                name: "Approval Lane".into(),
                subtitle: "Policy boundary".into(),
                description: "Surfaces Linux-backed actions that require explicit consent before execution.".into(),
            },
            AppEntry {
                name: "Wallet Hub".into(),
                subtitle: "Ownership surface".into(),
                description: "Keeps identity, balances, and signing visible without pretending they belong to the kernel.".into(),
            },
            AppEntry {
                name: "Ghost Console".into(),
                subtitle: "Native agent presence".into(),
                description: "Turns host runtime state into agent-readable, approval-aware orchestration inside SolOS.".into(),
            },
        ],
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
    }
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
