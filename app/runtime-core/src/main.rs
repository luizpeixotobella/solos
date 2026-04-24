use serde::{Deserialize, Serialize};
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
    intelligenceSummary: String,
    webStatusLabel: String,
    onboardingTitle: String,
    onboardingBody: String,
    onboardingUrl: String,
    onboardingStatus: String,
    pipelineStages: Vec<GhostPipelineStage>,
    lastResearch: GhostResearchSnapshot,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostPipelineStage {
    name: String,
    status: String,
    detail: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostResearchSnapshot {
    query: String,
    status: String,
    resultCount: usize,
    source: String,
    summary: String,
    citations: Vec<GhostCitation>,
}

#[derive(Serialize)]
struct GhostCitation {
    title: String,
    url: String,
    snippet: String,
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

#[derive(Clone)]
struct GhostIntelligenceState {
    available: bool,
    source: String,
    api_key_found: bool,
    onboarding_url: String,
    onboarding_status: String,
}

struct GhostBrain {
    intelligence: GhostIntelligenceState,
}

struct GhostResearchOutcome {
    query: String,
    status: String,
    source: String,
    summary: String,
    citations: Vec<GhostCitation>,
}

#[derive(Deserialize)]
struct BraveSearchResponse {
    #[serde(default)]
    web: Option<BraveWebResults>,
}

#[derive(Deserialize, Default)]
struct BraveWebResults {
    #[serde(default)]
    results: Vec<BraveWebResult>,
}

#[derive(Deserialize)]
struct BraveWebResult {
    #[serde(default)]
    title: String,
    #[serde(default)]
    url: String,
    #[serde(default)]
    description: String,
}

impl GhostBrain {
    fn new() -> Self {
        Self {
            intelligence: detect_brave_config(),
        }
    }

    fn process(&self, host: &HostRuntime, online: bool) -> (Vec<GhostPipelineStage>, GhostResearchOutcome) {
        let query = format!(
            "how to design a layered local AI agent with web search and approval flow for operating system shell"
        );

        let mut stages = Vec::new();
        stages.push(GhostPipelineStage {
            name: "data".into(),
            status: "active".into(),
            detail: format!(
                "Ghost reads host runtime facts from {} on {}, session {}, uptime {}, network {}.",
                host.os,
                host.hostname,
                host.sessionType,
                host.uptime,
                if online { "online" } else { "offline" }
            ),
        });

        let research = self.run_research(&query, online);

        stages.push(GhostPipelineStage {
            name: "results".into(),
            status: if research.status == "ready" { "ready" } else { "warning" }.into(),
            detail: format!(
                "Ghost converts raw runtime and web evidence into structured results. Source: {}.",
                research.source
            ),
        });

        stages.push(GhostPipelineStage {
            name: "algorithms".into(),
            status: "active".into(),
            detail: "Ghost applies a layered decision pipeline: ingest data, score relevance, summarize useful paths, and expose next actions inside SolOS.".into(),
        });

        (stages, research)
    }

    fn run_research(&self, query: &str, online: bool) -> GhostResearchOutcome {
        if !online {
            return GhostResearchOutcome {
                query: query.into(),
                status: "offline".into(),
                source: "network unavailable".into(),
                summary: "Ghost could not research the web because the host appears offline.".into(),
                citations: vec![],
            };
        }

        if !self.intelligence.available {
            return GhostResearchOutcome {
                query: query.into(),
                status: if self.intelligence.api_key_found { "degraded" } else { "unconfigured" }.into(),
                source: self.intelligence.source.clone(),
                summary: if self.intelligence.api_key_found {
                    "Ghost found a Brave key candidate but could not use it from the current runtime configuration.".into()
                } else {
                    "Ghost did not find a Brave Search API key in the known SolOS/runtime config paths or environment.".into()
                },
                citations: vec![],
            };
        }

        match brave_search(query, &self.intelligence.source) {
            Ok(citations) if !citations.is_empty() => {
                let summary = format!(
                    "Ghost found {} web references about layered agent design, web search grounding, and approval-aware orchestration.",
                    citations.len()
                );
                GhostResearchOutcome {
                    query: query.into(),
                    status: "ready".into(),
                    source: self.intelligence.source.clone(),
                    summary,
                    citations,
                }
            }
            Ok(_) => GhostResearchOutcome {
                query: query.into(),
                status: "empty".into(),
                source: self.intelligence.source.clone(),
                summary: "Ghost reached Brave Search but did not receive web results for the current query.".into(),
                citations: vec![],
            },
            Err(error) => GhostResearchOutcome {
                query: query.into(),
                status: "error".into(),
                source: self.intelligence.source.clone(),
                summary: format!("Ghost web research failed: {}", error),
                citations: vec![],
            },
        }
    }
}

fn main() {
    let host = detect_host_runtime();
    let online = detect_online();
    let ghost_brain = GhostBrain::new();
    let (ghost_pipeline, ghost_research) = ghost_brain.process(&host, online);
    let app_registry = build_app_registry();
    let approvals = build_approvals();
    let activity_feed = build_activity_feed(&host, online, app_registry.len(), &ghost_research);
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
        agentStatus: if ghost_research.status == "ready" {
            "Ghost active · web-grounded · approval-aware".into()
        } else {
            "Ghost active · local pipeline online".into()
        },
        runtimeMode: "runtime-intermediary".into(),
        runtimeSource: "Rust runtime-core mediates between Linux host services and the SolOS operating layer.".into(),
        runtimeRole: "Intermediary layer between Linux base system and SolOS operating layer".into(),
        mediationStatus: "Host facts detected and normalized into SolOS-facing runtime state".into(),
        home: HomeState {
            summaryTitle: "Ghost now has a first layered intelligence module".into(),
            summarySubtitle: "data + results = algorithms, grounded in host runtime and optional Brave web search.".into(),
            summaryBody: format!(
                "This build gives Ghost a first perceptron-like pipeline with stacked stages: runtime data ingestion, result synthesis, and algorithmic next-action output. Host state comes from {}, kernel {}, hostname {}, user {}, uptime {}. Web grounding currently reports: {}.",
                host.os, host.kernel, host.hostname, host.user, host.uptime, ghost_research.summary
            ),
            nextActionTitle: "Next useful move".into(),
            nextActionSubtitle: "Promote Ghost from research pipeline to task executor".into(),
            nextActionBody: "The next module should add task intents and tool actions so Ghost can turn researched guidance into explicit SolOS commands, approval requests, and user-visible outcomes.".into(),
        },
        ghost: GhostState {
            presenceLabel: "Ghost present in shell".into(),
            modeLabel: "layered-intelligence · approval-aware · web-grounded when configured".into(),
            thesisLabel: "Ghost should become a native orchestration layer that combines local runtime reality, web research, and explicit approvals into useful operating behavior.".into(),
            intelligenceSummary: "Perceptron-like layered pipeline: input data, synthesized results, then action-shaping algorithms.".into(),
            webStatusLabel: format!("Brave research status: {}", ghost_research.status),
            onboardingTitle: "Brave key onboarding".into(),
            onboardingBody: if ghost_brain.intelligence.available {
                "Ghost already has a repository-local Brave key configured for this SolOS checkout.".into()
            } else {
                format!(
                    "Ghost should send each SolOS user to Brave's key page, let them subscribe with their own account, then return and paste the key into solos/config/ghost.json so this repo stays isolated from your personal quota. Current status: {}.",
                    ghost_brain.intelligence.onboarding_status
                )
            },
            onboardingUrl: ghost_brain.intelligence.onboarding_url.clone(),
            onboardingStatus: ghost_brain.intelligence.onboarding_status.clone(),
            pipelineStages: ghost_pipeline,
            lastResearch: GhostResearchSnapshot {
                query: ghost_research.query,
                status: ghost_research.status,
                resultCount: ghost_research.citations.len(),
                source: ghost_research.source,
                summary: ghost_research.summary,
                citations: ghost_research.citations,
            },
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
            title: "Inspect Ghost data layer".into(),
            subtitle: "Input signals".into(),
            description: "Read host runtime facts, config, network state, and future user context as structured inputs for Ghost.".into(),
        },
        QuickAction {
            title: "Inspect Ghost results layer".into(),
            subtitle: "Synthesis".into(),
            description: "Combine local runtime evidence with Brave research results and condense them into useful summaries for the shell.".into(),
        },
        QuickAction {
            title: "Promote Ghost algorithms into actions".into(),
            subtitle: "Next module".into(),
            description: "Turn ranked Ghost conclusions into explicit SolOS tasks, approvals, and app-level actions instead of keeping them purely descriptive.".into(),
        },
    ]
}

fn build_activity_feed(
    host: &HostRuntime,
    online: bool,
    app_count: usize,
    research: &GhostResearchOutcome,
) -> Vec<ActivityEntry> {
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
            title: "Ghost layered pipeline online".into(),
            detail: "Ghost now runs a three-stage intelligence path: data ingestion, result synthesis, and algorithm shaping for next actions.".into(),
            status: "active".into(),
        },
        ActivityEntry {
            title: "Ghost web research state".into(),
            detail: format!(
                "Research query status is {}. {}",
                research.status, research.summary
            ),
            status: if research.status == "ready" {
                "ready".into()
            } else if online {
                "warning".into()
            } else {
                "warning".into()
            },
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
            id: "approval-ghost-web-access".into(),
            title: "Bind Ghost web research to the user's own Brave key".into(),
            description: "Ghost should open Brave's API key page for the SolOS user, let them pay or subscribe on their own account, then return and configure a repo-local key instead of sharing the developer key.".into(),
            requestedBy: "ghost-brain".into(),
            capability: "web.search.read".into(),
            scope: "ghost onboarding -> Brave key acquisition -> solos/config/ghost.json".into(),
            risk: "medium".into(),
            status: "pending".into(),
            createdAt: "ghost-module-bootstrap".into(),
        },
        ApprovalEntry {
            id: "approval-ghost-task-execution".into(),
            title: "Connect Ghost conclusions to executable tasks".into(),
            description: "Prepare the next module so Ghost can transform ranked conclusions into app launches, commands, and approval requests inside SolOS.".into(),
            requestedBy: "ghost-brain".into(),
            capability: "task.intent.dispatch".into(),
            scope: "Ghost pipeline -> SolOS task/action router".into(),
            risk: "high".into(),
            status: "pending".into(),
            createdAt: "ghost-module-bootstrap".into(),
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
            subtitle: "Layered intelligence module".into(),
            description: "Turns runtime data plus optional Brave research into structured results and next-step algorithms inside SolOS.".into(),
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

fn detect_brave_config() -> GhostIntelligenceState {
    let env_candidates = [
        "BRAVE_API_KEY",
        "BRAVE_SEARCH_API_KEY",
        "SOLOS_BRAVE_API_KEY",
    ];

    for name in env_candidates {
        if let Ok(value) = env::var(name) {
            if !value.trim().is_empty() {
                return GhostIntelligenceState {
                    available: true,
                    source: format!("env:{}", name),
                    api_key_found: true,
                    onboarding_url: read_onboarding_url().unwrap_or_else(|| "https://api-dashboard.search.brave.com/app/keys".into()),
                    onboarding_status: "configured".into(),
                };
            }
        }
    }

    let file_candidates = brave_key_file_candidates();

    for path in file_candidates {
        if let Some(value) = read_brave_key_from_path(path) {
            if !value.trim().is_empty() {
                return GhostIntelligenceState {
                    available: true,
                    source: format!("file:{}", path),
                    api_key_found: true,
                    onboarding_url: read_onboarding_url().unwrap_or_else(|| "https://api-dashboard.search.brave.com/app/keys".into()),
                    onboarding_status: "configured".into(),
                };
            }
        }
    }

    GhostIntelligenceState {
        available: false,
        source: "missing-config".into(),
        api_key_found: false,
        onboarding_url: read_onboarding_url().unwrap_or_else(|| "https://api-dashboard.search.brave.com/app/keys".into()),
        onboarding_status: read_onboarding_status().unwrap_or_else(|| "needs-user-key".into()),
    }
}

fn brave_search(query: &str, source: &str) -> Result<Vec<GhostCitation>, String> {
    let api_key = resolve_brave_key().ok_or_else(|| format!("Brave API key unavailable from {}", source))?;
    let encoded_query = url_encode(query);
    let command = format!(
        "curl -fsSL --max-time 12 -H 'Accept: application/json' -H 'X-Subscription-Token: {}' 'https://api.search.brave.com/res/v1/web/search?q={}&count=5'",
        escape_single_quotes(&api_key),
        encoded_query
    );

    let output = Command::new("sh")
        .arg("-lc")
        .arg(command)
        .output()
        .map_err(|e| e.to_string())?;

    if !output.status.success() {
        let stderr = String::from_utf8_lossy(&output.stderr).trim().to_string();
        return Err(if stderr.is_empty() {
            format!("curl exited with status {}", output.status)
        } else {
            stderr
        });
    }

    let body: BraveSearchResponse = serde_json::from_slice(&output.stdout).map_err(|e| e.to_string())?;
    let citations = body
        .web
        .unwrap_or_default()
        .results
        .into_iter()
        .take(3)
        .map(|item| GhostCitation {
            title: item.title,
            url: item.url,
            snippet: item.description,
        })
        .collect();

    Ok(citations)
}

fn resolve_brave_key() -> Option<String> {
    let env_candidates = [
        "BRAVE_API_KEY",
        "BRAVE_SEARCH_API_KEY",
        "SOLOS_BRAVE_API_KEY",
    ];

    for name in env_candidates {
        if let Ok(value) = env::var(name) {
            if !value.trim().is_empty() {
                return Some(value);
            }
        }
    }

    for path in brave_key_file_candidates() {
        if let Some(value) = read_brave_key_from_path(path) {
            if !value.trim().is_empty() {
                return Some(value);
            }
        }
    }

    None
}

fn read_brave_key_from_path(path: &str) -> Option<String> {
    let content = fs::read_to_string(path).ok()?;

    if path.ends_with(".json") {
        let json: serde_json::Value = serde_json::from_str(&content).ok()?;
        let pointer_candidates = [
            "/braveApiKey",
            "/brave/apiKey",
            "/ghost/braveApiKey",
            "/ghost/brave/apiKey",
            "/search/brave/apiKey",
            "/ghost/intelligence/webSearch/apiKey",
            "/ghost/webSearch/apiKey",
        ];

        for pointer in pointer_candidates {
            if let Some(value) = json.pointer(pointer).and_then(|v| v.as_str()) {
                if !value.trim().is_empty() {
                    return Some(value.to_string());
                }
            }
        }

        return None;
    }

    for line in content.lines() {
        let trimmed = line.trim();
        if trimmed.starts_with('#') || trimmed.is_empty() {
            continue;
        }

        for key in ["BRAVE_API_KEY", "BRAVE_SEARCH_API_KEY", "SOLOS_BRAVE_API_KEY"] {
            let prefix = format!("{}=", key);
            if let Some(value) = trimmed.strip_prefix(&prefix) {
                return Some(value.trim_matches('"').trim_matches('\'').to_string());
            }
        }
    }

    None
}

fn brave_key_file_candidates() -> Vec<&'static str> {
    vec![
        "./config/ghost.json",
        "../../config/ghost.json",
        "./solos/config/ghost.json",
        "./config/solos.json",
        "./config/runtime.json",
        "./.env",
        "./.env.local",
        "./app/shell/.env",
        "./app/shell/.env.local",
    ]
}

fn read_onboarding_url() -> Option<String> {
    let path = "./solos/config/ghost.json";
    let content = fs::read_to_string(path).ok()?;
    let json: serde_json::Value = serde_json::from_str(&content).ok()?;
    json.pointer("/ghost/intelligence/webSearch/onboardingUrl")
        .and_then(|v| v.as_str())
        .map(|v| v.to_string())
}

fn read_onboarding_status() -> Option<String> {
    let path = "./solos/config/ghost.json";
    let content = fs::read_to_string(path).ok()?;
    let json: serde_json::Value = serde_json::from_str(&content).ok()?;
    json.pointer("/ghost/intelligence/webSearch/status")
        .and_then(|v| v.as_str())
        .map(|v| v.to_string())
}

fn escape_single_quotes(value: &str) -> String {
    value.replace('\'', "'\\''")
}

fn url_encode(value: &str) -> String {
    let mut encoded = String::new();
    for byte in value.bytes() {
        match byte {
            b'A'..=b'Z' | b'a'..=b'z' | b'0'..=b'9' | b'-' | b'_' | b'.' | b'~' => encoded.push(byte as char),
            b' ' => encoded.push_str("%20"),
            _ => encoded.push_str(&format!("%{:02X}", byte)),
        }
    }
    encoded
}
