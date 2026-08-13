use serde::{Deserialize, Serialize};
use std::env;
use std::fs;
use std::path::Path;
use std::process::Command;
use std::time::Duration;
use std::time::{SystemTime, UNIX_EPOCH};

#[derive(Serialize)]
#[allow(non_snake_case)]
struct RuntimeSnapshot {
    schemaVersion: String,
    productVersion: String,
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
    heartPass: HeartPassState,
    quickActions: Vec<QuickAction>,
    activityFeed: Vec<ActivityEntry>,
    approvals: Vec<ApprovalEntry>,
    apps: Vec<AppEntry>,
    hostRuntime: HostRuntime,
    systemStatus: SystemStatus,
    capabilityManifest: CapabilityManifest,
    traceEvaluation: TraceEvaluation,
    mediatedAction: MediatedAction,
    walletSession: WalletSession,
    memoryPolicy: MemoryPolicy,
    quotaProxy: QuotaProxyContract,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct CapabilityManifest {
    schema: String,
    defaultPolicy: String,
    capabilities: Vec<CapabilityDefinition>,
}

#[derive(Serialize)]
struct CapabilityDefinition {
    id: String,
    scopes: Vec<String>,
    risk: String,
    approval: String,
    executable: bool,
    audit: String,
}

#[derive(Serialize, Deserialize, Clone)]
#[allow(non_snake_case)]
struct TraceRecord {
    traceId: String,
    requestClass: String,
    selectedRoute: String,
    expectedRoute: String,
    outcome: String,
    approvalRequired: bool,
    quotaCost: u32,
    correctedRoute: Option<String>,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct TraceEvaluation {
    schema: String,
    storePath: String,
    total: usize,
    accepted: usize,
    rejected: usize,
    corrected: usize,
    passRate: f64,
    records: Vec<TraceRecord>,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct MediatedAction {
    id: String,
    capability: String,
    target: String,
    status: String,
    approvalRequired: bool,
    result: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct WalletSession {
    status: String,
    proofMode: String,
    addressPersistence: String,
    sponsoredCallsAllowed: bool,
    failureReason: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct MemoryPolicy {
    defaultRetention: String,
    classes: Vec<MemoryClass>,
}

#[derive(Serialize)]
struct MemoryClass {
    name: String,
    retention: String,
    revocation: String,
    sensitive: bool,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct QuotaProxyContract {
    endpoint: String,
    status: String,
    requiresSignedHolderProof: bool,
    idempotencyRequired: bool,
    providerNeutral: bool,
    fallback: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct HeartPassState {
    title: String,
    status: String,
    network: String,
    tokenStandard: String,
    contract: String,
    tokenId: String,
    openSeaUrl: String,
    summary: String,
    nextStep: String,
    walletAddress: String,
    ownerAddress: String,
    verificationStatus: String,
    lastCheckedAt: String,
    configPath: String,
    pulsoRewardsClaimed: usize,
    quotaLayer: QuotaLayerState,
    capabilities: Vec<String>,
}

#[derive(Serialize, Deserialize, Default)]
#[allow(non_snake_case)]
struct HeartPassConfig {
    schema: String,
    title: String,
    network: String,
    tokenStandard: String,
    contract: String,
    tokenId: String,
    openSeaUrl: String,
    walletAddress: String,
    ownerAddress: String,
    verificationStatus: String,
    lastCheckedAt: String,
    notes: String,
    #[serde(default)]
    claimedPulsoRedemptions: Vec<String>,
    #[serde(default)]
    quotaLayer: QuotaLayerConfig,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct QuotaConsumptionResult {
    schema: String,
    consumed: u32,
    usedQueries: u32,
    remainingQueries: u32,
    status: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct QuotaLayerState {
    title: String,
    status: String,
    mode: String,
    period: String,
    includedQueries: u32,
    usedQueries: u32,
    remainingQueries: u32,
    fallback: String,
    usageSource: String,
    lastSync: String,
    resetPolicy: String,
    summary: String,
    nextStep: String,
}

#[derive(Serialize, Deserialize, Clone, Default)]
#[allow(non_snake_case)]
struct QuotaLayerConfig {
    status: String,
    mode: String,
    period: String,
    includedQueries: u32,
    usedQueries: u32,
    remainingQueries: u32,
    fallback: String,
    usageSource: String,
    lastSync: String,
    resetPolicy: String,
    notes: String,
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
    intentsTitle: String,
    intentsSummary: String,
    intents: Vec<GhostIntent>,
    pipelineStages: Vec<GhostPipelineStage>,
    lastResearch: GhostResearchSnapshot,
    initiation: GhostInitiationState,
    knowledge: GhostKnowledgeSnapshot,
    languageSupport: GhostLanguageSupport,
    operationalReadiness: GhostOperationalReadiness,
    requestClassifier: GhostRequestClassifier,
    actionTrace: GhostActionTrace,
    routeExplanation: GhostRouteExplanation,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostRequestClassifier {
    title: String,
    summary: String,
    exampleRequest: String,
    classes: Vec<GhostRequestClass>,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostRequestClass {
    name: String,
    status: String,
    safetyLevel: String,
    requiredTools: String,
    approvalNeeds: String,
    quotaCost: String,
    route: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostActionTrace {
    traceId: String,
    request: String,
    data: String,
    resultTarget: String,
    algorithmRoute: String,
    outcome: String,
    quotaCost: String,
    approvalRequired: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostRouteExplanation {
    selectedClass: String,
    selectedRoute: String,
    safetyLevel: String,
    explanation: String,
    approvalPolicy: String,
    quotaPolicy: String,
    nextStep: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostLanguageSupport {
    status: String,
    summary: String,
    primaryLanguages: Vec<String>,
    operatingPrinciples: Vec<String>,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostOperationalReadiness {
    status: String,
    summary: String,
    pillars: Vec<GhostReadinessPillar>,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostReadinessPillar {
    name: String,
    status: String,
    evidence: String,
    nextAction: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostInitiationState {
    status: String,
    budgetMinutes: String,
    databasePath: String,
    summary: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostKnowledgeSnapshot {
    topicCount: usize,
    targetTopicCount: usize,
    topics: Vec<GhostKnowledgeTopicSnapshot>,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
struct GhostKnowledgeTopicSnapshot {
    name: String,
    status: String,
    summary: String,
    sourceCount: usize,
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
#[allow(non_snake_case)]
struct GhostIntent {
    name: String,
    status: String,
    reason: String,
    nextAction: String,
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

#[derive(Serialize, Deserialize, Default)]
#[allow(non_snake_case)]
struct GhostKnowledgeDatabase {
    schema: String,
    purpose: String,
    createdAt: String,
    updatedAt: String,
    topics: Vec<GhostKnowledgeTopic>,
}

#[derive(Serialize, Deserialize, Clone)]
#[allow(non_snake_case)]
struct GhostKnowledgeTopic {
    id: String,
    name: String,
    query: String,
    summary: String,
    lastHarvestedAt: String,
    sources: Vec<GhostKnowledgeSource>,
}

#[derive(Serialize, Deserialize, Clone)]
struct GhostKnowledgeSource {
    title: String,
    url: String,
    snippet: String,
}

struct GhostInitiationOutcome {
    status: String,
    summary: String,
    database_path: String,
    topics: Vec<GhostKnowledgeTopic>,
}

fn build_ghost_operational_readiness(
    research: &GhostResearchOutcome,
    initiation: &GhostInitiationOutcome,
    heart_pass_verified: bool,
) -> GhostOperationalReadiness {
    let grounding_ready = research.status == "ready";
    let memory_ready = !initiation.topics.is_empty();
    let approvals_ready = heart_pass_verified;
    let status = if grounding_ready && memory_ready && approvals_ready {
        "operator-preview"
    } else if memory_ready || grounding_ready {
        "partially-ready"
    } else {
        "foundation"
    };

    GhostOperationalReadiness {
        status: status.into(),
        summary: "Ghost is now measured against the capabilities modern agents need before they should act inside an operating layer: grounded retrieval, durable memory, tool boundaries, approvals, observability, and user-language mediation.".into(),
        pillars: vec![
            GhostReadinessPillar {
                name: "Grounded research and RAG".into(),
                status: if grounding_ready { "ready" } else { "needs-configuration" }.into(),
                evidence: format!(
                    "Web grounding status is {} through {}; local knowledge cache has {} topic(s).",
                    research.status,
                    research.source,
                    initiation.topics.len()
                ),
                nextAction: if grounding_ready {
                    "Use cited web evidence for fresh claims and keep source snippets visible in the shell.".into()
                } else {
                    "Configure the user's Brave key or another user-owned retrieval provider before external claims are treated as grounded.".into()
                },
            },
            GhostReadinessPillar {
                name: "Long-term memory".into(),
                status: if memory_ready { "seeded" } else { "waiting" }.into(),
                evidence: format!(
                    "Repo-local Ghost knowledge database tracks {} of {} initiation topics.",
                    initiation.topics.len(),
                    ghost_knowledge_curriculum().len()
                ),
                nextAction: "Promote the cache into scoped memory classes: session facts, durable user preferences, project docs, and revocable sensitive context.".into(),
            },
            GhostReadinessPillar {
                name: "Tool and MCP boundary".into(),
                status: "designed".into(),
                evidence: "Modern agent platforms expose tools and MCP-style connectors; SolOS keeps Ghost behind runtime-mediated capability descriptions instead of direct host control.".into(),
                nextAction: "Add a tool manifest with read/write/sensitive scopes, default-deny execution, and per-tool audit records.".into(),
            },
            GhostReadinessPillar {
                name: "Human approval lane".into(),
                status: if approvals_ready { "gated" } else { "visible" }.into(),
                evidence: if approvals_ready {
                    "Heart Pass state allows gated Ghost onboarding and the approval queue is surfaced in the shell.".into()
                } else {
                    "Approval queue is visible, but Heart Pass verification is still required for gated Ghost onboarding.".into()
                },
                nextAction: "Require explicit approval for account-linked, billable, wallet, filesystem write, shell, network, and public-posting actions.".into(),
            },
            GhostReadinessPillar {
                name: "Observability and evals".into(),
                status: "planned".into(),
                evidence: "The runtime snapshot exposes state, citations, approvals, and activity feed, but does not yet preserve traces or task outcome grades.".into(),
                nextAction: "Persist trace summaries for prompt, retrieval, tool calls, approval decision, action result, and user-visible outcome.".into(),
            },
            GhostReadinessPillar {
                name: "Language and tone mediation".into(),
                status: "planned-core-capability".into(),
                evidence: "Ghost already models multilingual support as an operating capability rather than cosmetic localization.".into(),
                nextAction: "Route intent, approval explanations, retrieved evidence, and final responses through the user's active language and register.".into(),
            },
        ],
    }
}

fn build_ghost_request_classifier(
    online: bool,
    research: &GhostResearchOutcome,
    heart_pass: &HeartPassState,
) -> GhostRequestClassifier {
    let web_grounded = online && research.status == "ready";
    let quota_active = heart_pass.quotaLayer.status != "verification-required"
        && heart_pass.quotaLayer.remainingQueries > 0;
    let research_quota_cost = if quota_active {
        "1 Heart Pass holder query when web research executes; BYOK fallback remains available"
    } else {
        "0 local queries now; web research must use BYOK until pass quota is active"
    };

    GhostRequestClassifier {
        title: "Ghost request classifier".into(),
        summary: "The first W3Schools-inspired classifier turns user language into visible request classes before Ghost chooses a route, asks for approval, or spends quota.".into(),
        exampleRequest: "Turn the archived W3Schools AI material into the next AI Ghost implementation slice.".into(),
        classes: vec![
            GhostRequestClass {
                name: "research request".into(),
                status: if web_grounded { "ready" } else { "byok-or-local-only" }.into(),
                safetyLevel: "low-to-medium".into(),
                requiredTools: "local cache, optional web.search.read".into(),
                approvalNeeds: "approval only when the route consumes paid/sponsored quota or opens account-linked setup".into(),
                quotaCost: research_quota_cost.into(),
                route: "answer from local docs first; use Brave-grounded retrieval only when configured and justified".into(),
            },
            GhostRequestClass {
                name: "system action request".into(),
                status: "approval-gated".into(),
                safetyLevel: "high".into(),
                requiredTools: "task.intent.dispatch, filesystem or command capability after policy review".into(),
                approvalNeeds: "explicit user approval before shell commands, filesystem writes, host changes, or app launches".into(),
                quotaCost: "0 model/web quota by itself; execution trace must still be recorded".into(),
                route: "classify, explain, request approval, then execute through a mediated task boundary".into(),
            },
            GhostRequestClass {
                name: "wallet/pass request".into(),
                status: heart_pass.verificationStatus.clone(),
                safetyLevel: "high".into(),
                requiredTools: "wallet identity, Polygon read call, optional signature lane".into(),
                approvalNeeds: "explicit approval for signatures, payments, account linking, or wallet address persistence".into(),
                quotaCost: "0 Ghost research queries; may unlock holder quota after verification".into(),
                route: "keep ownership, verification, and quota state visible in Wallet and Agent surfaces".into(),
            },
            GhostRequestClass {
                name: "documentation task".into(),
                status: "ready".into(),
                safetyLevel: "low".into(),
                requiredTools: "repo docs and structured runtime state".into(),
                approvalNeeds: "no approval for local docs; approval required before public publishing".into(),
                quotaCost: "0".into(),
                route: "update docs alongside code so product doctrine and implementation do not drift".into(),
            },
            GhostRequestClass {
                name: "content/publishing task".into(),
                status: "approval-required".into(),
                safetyLevel: "medium".into(),
                requiredTools: "CMS, media, social connectors when configured".into(),
                approvalNeeds: "approval before posting publicly or sending from the user's accounts".into(),
                quotaCost: "0-1 research query only when fresh external facts are needed".into(),
                route: "draft locally, attach evidence/media, then wait for explicit publish authorization".into(),
            },
            GhostRequestClass {
                name: "risky external action".into(),
                status: "default-deny".into(),
                safetyLevel: "critical".into(),
                requiredTools: "network, account, payment, public-send, or destructive host capability".into(),
                approvalNeeds: "must be stopped or escalated before execution".into(),
                quotaCost: "not relevant until safety route is accepted".into(),
                route: "refuse unsafe requests or move into a narrow approval lane with full impact text".into(),
            },
            GhostRequestClass {
                name: "memory/update request".into(),
                status: "ready-with-scope".into(),
                safetyLevel: "medium".into(),
                requiredTools: "scoped memory store and session notes".into(),
                approvalNeeds: "approval or strong user intent before durable personal memory is written".into(),
                quotaCost: "0".into(),
                route: "capture only durable, useful context with source and revocation path".into(),
            },
            GhostRequestClass {
                name: "unclear request".into(),
                status: "clarify-first".into(),
                safetyLevel: "unknown".into(),
                requiredTools: "none until intent is resolved".into(),
                approvalNeeds: "ask a concise clarifying question when a safe assumption would be risky".into(),
                quotaCost: "0".into(),
                route: "hold execution, explain ambiguity, and ask for the missing constraint".into(),
            },
        ],
    }
}

fn build_ghost_action_trace(heart_pass: &HeartPassState) -> GhostActionTrace {
    GhostActionTrace {
        traceId: "ghost-trace-w3schools-classification-v1".into(),
        request: "Turn the archived W3Schools AI material into the next AI Ghost implementation slice.".into(),
        data: "W3Schools AI archive, SolOS runtime snapshot, Heart Pass quota contract, Ghost readiness doctrine, and local repository state.".into(),
        resultTarget: "A visible Ghost classifier, action trace, and route explanation in Agent/Ghost without public posting or account-linked actions.".into(),
        algorithmRoute: "classify request -> score safety/quota/approval needs -> choose local implementation route -> document the outcome.".into(),
        outcome: "local-implementation-prepared".into(),
        quotaCost: if heart_pass.quotaLayer.status == "verification-required" {
            "0 holder queries; no web research is consumed because the archive already exists and quota is verification-gated.".into()
        } else {
            "0 holder queries for this local implementation slice; future web-grounded research will decrement quota.".into()
        },
        approvalRequired: "No external approval for local repo work; explicit approval remains required before public posting, wallet signing, paid provider use, or destructive host actions.".into(),
    }
}

fn build_ghost_route_explanation(
    online: bool,
    research: &GhostResearchOutcome,
    heart_pass: &HeartPassState,
) -> GhostRouteExplanation {
    let selected_route = if online && research.status == "ready" {
        "local-implementation-with-grounded-research-available"
    } else {
        "local-implementation-from-archived-evidence"
    };

    GhostRouteExplanation {
        selectedClass: "documentation task + system action request".into(),
        selectedRoute: selected_route.into(),
        safetyLevel: "medium".into(),
        explanation: "The request asks Ghost to continue product implementation from an already archived learning source. The safe route is local code/docs work with a visible trace, not fresh web spending or public publishing.".into(),
        approvalPolicy: "Repo edits and local builds can proceed in the development workspace. External messages, social posts, wallet signatures, paid-provider calls, and destructive commands stay approval-gated.".into(),
        quotaPolicy: format!(
            "Heart Pass quota is {} with {} remaining in {} mode; this route spends 0 research queries.",
            heart_pass.quotaLayer.status,
            heart_pass.quotaLayer.remainingQueries,
            heart_pass.quotaLayer.mode
        ),
        nextStep: "Persist trace outcomes and accepted/rejected examples so Ghost can compare future routes against expected behavior.".into(),
    }
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

    fn process(
        &self,
        host: &HostRuntime,
        online: bool,
    ) -> (
        Vec<GhostPipelineStage>,
        GhostResearchOutcome,
        Vec<GhostIntent>,
        GhostInitiationOutcome,
    ) {
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
            status: if research.status == "ready" {
                "ready"
            } else {
                "warning"
            }
            .into(),
            detail: format!(
                "Ghost converts raw runtime and web evidence into structured results. Source: {}.",
                research.source
            ),
        });

        stages.push(GhostPipelineStage {
            name: "algorithms".into(),
            status: "active".into(),
            detail: "Ghost applies the AI-era inversion: data plus observed/desired results shape algorithms for routing, approvals, and next actions inside SolOS.".into(),
        });

        stages.push(GhostPipelineStage {
            name: "classification".into(),
            status: "active".into(),
            detail: "Ghost now exposes a deterministic request classifier before action: class, safety, required tools, approval needs, quota cost, and route.".into(),
        });

        let initiation = self.run_initiation(online);

        stages.push(GhostPipelineStage {
            name: "knowledge".into(),
            status: initiation.status.clone(),
            detail: format!(
                "Ghost keeps a repo-local initiation knowledge database at {}. {}",
                initiation.database_path, initiation.summary
            ),
        });

        stages.push(GhostPipelineStage {
            name: "human-language-support".into(),
            status: "planned".into(),
            detail: "Ghost should treat major human languages as first-class operating context: detect language, answer in the user's language, preserve meaning across translation, and use data-mined cultural context without flattening local nuance.".into(),
        });

        let intents = self.build_intents(host, online, &research);
        (stages, research, intents, initiation)
    }

    fn run_research(&self, query: &str, online: bool) -> GhostResearchOutcome {
        if !online {
            return GhostResearchOutcome {
                query: query.into(),
                status: "offline".into(),
                source: "network unavailable".into(),
                summary: "Ghost could not research the web because the host appears offline."
                    .into(),
                citations: vec![],
            };
        }

        if !self.intelligence.available {
            return GhostResearchOutcome {
                query: query.into(),
                status: if self.intelligence.api_key_found {
                    "degraded"
                } else {
                    "unconfigured"
                }
                .into(),
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

    fn build_intents(
        &self,
        host: &HostRuntime,
        online: bool,
        research: &GhostResearchOutcome,
    ) -> Vec<GhostIntent> {
        vec![
            GhostIntent {
                name: "answer-directly".into(),
                status: "ready".into(),
                reason: format!(
                    "Ghost already has host facts for {} on {} and can answer local environment questions without leaving SolOS.",
                    host.user, host.hostname
                ),
                nextAction: "Use local runtime state first when the request is about host/session status.".into(),
            },
            GhostIntent {
                name: "research-web".into(),
                status: if online && research.status == "ready" { "ready" } else { "waiting" }.into(),
                reason: format!(
                    "Ghost web grounding is {} via {}.",
                    research.status, research.source
                ),
                nextAction: if online && research.status == "ready" {
                    "Use Brave-backed evidence when the user asks how to do something or needs fresh external context.".into()
                } else {
                    "Require a valid Brave key and network before treating web research as available.".into()
                },
            },
            GhostIntent {
                name: "request-approval".into(),
                status: "ready".into(),
                reason: "Ghost should escalate from guidance to explicit approval when an intent touches system actions, wallets, or account-linked operations.".into(),
                nextAction: "Route risky or billable actions into the approval lane instead of silently executing them.".into(),
            },
        ]
    }

    fn run_initiation(&self, online: bool) -> GhostInitiationOutcome {
        let database_path = ghost_knowledge_database_path();
        let mut database = load_ghost_knowledge_database(&database_path);

        if !online {
            return GhostInitiationOutcome {
                status: "offline".into(),
                summary: "Initiation paused because the host appears offline.".into(),
                database_path,
                topics: database.topics,
            };
        }

        if !self.intelligence.available {
            return GhostInitiationOutcome {
                status: "waiting-for-brave-key".into(),
                summary: "Initiation will start after the user configures a Brave Search key."
                    .into(),
                database_path,
                topics: database.topics,
            };
        }

        let curriculum = ghost_knowledge_curriculum();
        let mut harvested_this_run = 0usize;
        let max_topics_per_run = 1usize;

        for seed in &curriculum {
            if harvested_this_run >= max_topics_per_run {
                break;
            }

            if database.topics.iter().any(|topic| topic.id == seed.id) {
                continue;
            }

            match brave_search(&seed.query, &self.intelligence.source) {
                Ok(citations) if !citations.is_empty() => {
                    let sources: Vec<GhostKnowledgeSource> = citations
                        .into_iter()
                        .map(|citation| GhostKnowledgeSource {
                            title: citation.title,
                            url: citation.url,
                            snippet: citation.snippet,
                        })
                        .collect();

                    database.topics.push(GhostKnowledgeTopic {
                        id: seed.id.into(),
                        name: seed.name.into(),
                        query: seed.query.clone(),
                        summary: seed.summary.into(),
                        lastHarvestedAt: unix_timestamp_string(),
                        sources,
                    });
                    harvested_this_run += 1;
                }
                Ok(_) | Err(_) => {
                    // Keep initiation resilient: one bad topic should not block the shell load.
                    continue;
                }
            }
        }

        if harvested_this_run > 0 {
            database.updatedAt = unix_timestamp_string();
            let _ = save_ghost_knowledge_database(&database_path, &database);
        }

        let status = if database.topics.len() >= curriculum.len() {
            "ready"
        } else if harvested_this_run > 0 {
            "initiating"
        } else {
            "cached"
        };

        GhostInitiationOutcome {
            status: status.into(),
            summary: format!(
                "{} of {} initiation topics cached; harvested {} topic(s) this runtime pass.",
                database.topics.len(),
                curriculum.len(),
                harvested_this_run
            ),
            database_path,
            topics: database.topics,
        }
    }
}

fn main() {
    if env::args().nth(1).as_deref() == Some("consume-ghost-query") {
        let amount = env::args()
            .nth(2)
            .and_then(|value| value.parse::<u32>().ok())
            .unwrap_or(1);
        match consume_ghost_queries(amount) {
            Ok(result) => {
                println!(
                    "{}",
                    serde_json::to_string_pretty(&result).unwrap_or_default()
                );
                return;
            }
            Err(error) => {
                eprintln!("{error}");
                std::process::exit(1);
            }
        }
    }

    let host = detect_host_runtime();
    let online = detect_online();
    let ghost_brain = GhostBrain::new();
    let heart_pass = build_heart_pass_state();
    let (ghost_pipeline, ghost_research, ghost_intents, ghost_initiation) =
        ghost_brain.process(&host, online);
    let heart_pass_verified = heart_pass.verificationStatus == "verified-holder";
    let ghost_onboarding_status = if heart_pass_verified {
        ghost_brain.intelligence.onboarding_status.clone()
    } else {
        "heart-pass-required".into()
    };
    let ghost_onboarding_body = if !heart_pass_verified {
        format!(
            "Ghost/Brave onboarding is gated by the SolOS Heart Pass. Configure a Polygon wallet in Wallet Hub and verify it holds token #{} before saving a Brave key. Current Heart Pass status: {}.",
            heart_pass.tokenId, heart_pass.verificationStatus
        )
    } else if ghost_brain.intelligence.available {
        "Heart Pass verified. Ghost already has a repository-local Brave key configured for this SolOS checkout.".into()
    } else {
        format!(
            "Heart Pass verified. Ghost should send this SolOS user to Brave's key page, let them subscribe with their own account, then return and save the key in the ignored solos/config/ghost.local.json file. Current status: {}.",
            ghost_brain.intelligence.onboarding_status
        )
    };
    let app_registry = build_app_registry();
    let approvals = build_approvals();
    let activity_feed = build_activity_feed(
        &host,
        online,
        app_registry.len(),
        &ghost_research,
        &heart_pass,
    );
    let quick_actions = build_quick_actions();
    let approvals_count = approvals.len();
    let notifications_count = activity_feed.len();
    let ghost_operational_readiness =
        build_ghost_operational_readiness(&ghost_research, &ghost_initiation, heart_pass_verified);
    let ghost_request_classifier =
        build_ghost_request_classifier(online, &ghost_research, &heart_pass);
    let ghost_action_trace = build_ghost_action_trace(&heart_pass);
    let ghost_route_explanation =
        build_ghost_route_explanation(online, &ghost_research, &heart_pass);
    let trace_evaluation = load_trace_evaluation();
    let wallet_session = build_wallet_session(&heart_pass);

    let snapshot = RuntimeSnapshot {
        schemaVersion: "solos.runtime.snapshot.v1".into(),
        productVersion: "1.0.0-rc1".into(),
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
            summarySubtitle: "AI-era synthesis: data + results = algorithms, grounded in host runtime, outcomes, and optional Brave web search.".into(),
            summaryBody: format!(
                "This build gives Ghost a first perceptron-lineage pipeline with stacked stages: runtime data ingestion, result/outcome synthesis, and algorithmic next-action output. Classical code starts from algorithms + data = results; Ghost is being oriented toward data + results = algorithms. Host state comes from {}, kernel {}, hostname {}, user {}, uptime {}. Web grounding currently reports: {}.",
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
            intelligenceSummary: "Perceptron-lineage layered pipeline: data plus results synthesize action-shaping algorithms.".into(),
            webStatusLabel: if heart_pass_verified {
                format!("Brave research status: {}", ghost_research.status)
            } else {
                format!("Brave onboarding gated by Heart Pass: {}", heart_pass.verificationStatus)
            },
            onboardingTitle: "Heart Pass gated Brave onboarding".into(),
            onboardingBody: ghost_onboarding_body,
            onboardingUrl: ghost_brain.intelligence.onboarding_url.clone(),
            onboardingStatus: ghost_onboarding_status,
            intentsTitle: "Ghost intents".into(),
            intentsSummary: "First intent router: answer locally, research externally when grounded, and request approval for sensitive actions.".into(),
            intents: ghost_intents,
            pipelineStages: ghost_pipeline,
            lastResearch: GhostResearchSnapshot {
                query: ghost_research.query,
                status: ghost_research.status,
                resultCount: ghost_research.citations.len(),
                source: ghost_research.source,
                summary: ghost_research.summary,
                citations: ghost_research.citations,
            },
            initiation: GhostInitiationState {
                status: ghost_initiation.status.clone(),
                budgetMinutes: "5-10 minutes, incremental and cached".into(),
                databasePath: ghost_initiation.database_path.clone(),
                summary: ghost_initiation.summary.clone(),
            },
            knowledge: GhostKnowledgeSnapshot {
                topicCount: ghost_initiation.topics.len(),
                targetTopicCount: ghost_knowledge_curriculum().len(),
                topics: ghost_initiation
                    .topics
                    .iter()
                    .map(|topic| GhostKnowledgeTopicSnapshot {
                        name: topic.name.clone(),
                        status: "cached".into(),
                        summary: topic.summary.clone(),
                        sourceCount: topic.sources.len(),
                    })
                    .collect(),
            },
            languageSupport: GhostLanguageSupport {
                status: "planned-core-capability".into(),
                summary: "Ghost is being oriented toward multilingual human fluency: language detection, response in the user's language, translation as mediation, and culturally aware retrieval over the major languages of the world.".into(),
                primaryLanguages: vec![
                    "English".into(),
                    "Portuguese".into(),
                    "Spanish".into(),
                    "French".into(),
                    "German".into(),
                    "Italian".into(),
                    "Arabic".into(),
                    "Hindi".into(),
                    "Bengali".into(),
                    "Mandarin Chinese".into(),
                    "Japanese".into(),
                    "Korean".into(),
                    "Russian".into(),
                    "Indonesian".into(),
                    "Turkish".into(),
                ],
                operatingPrinciples: vec![
                    "Prefer the user's current language when replying or asking clarifying questions.".into(),
                    "Use translation as a mediated operating capability, not as cosmetic localization.".into(),
                    "Keep source-language citations and translated summaries linked when web/data mining is involved.".into(),
                    "Treat cultural context, idiom, tone, and register as part of Ghost's intelligence layer.".into(),
                ],
            },
            operationalReadiness: ghost_operational_readiness,
            requestClassifier: ghost_request_classifier,
            actionTrace: ghost_action_trace,
            routeExplanation: ghost_route_explanation,
        },
        heartPass: heart_pass,
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
        capabilityManifest: build_capability_manifest(),
        traceEvaluation: trace_evaluation,
        mediatedAction: build_mediated_action(),
        walletSession: wallet_session,
        memoryPolicy: build_memory_policy(),
        quotaProxy: build_quota_proxy_contract(),
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
    [
        "/sys/class/net/wlan0/operstate",
        "/sys/class/net/eth0/operstate",
    ]
    .iter()
    .any(|path| {
        fs::read_to_string(path)
            .map(|v| v.trim() == "up")
            .unwrap_or(false)
    }) || run("ip", &["route", "show", "default"]).is_some()
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
    heart_pass: &HeartPassState,
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
            detail: "Ghost now runs the intelligence inversion path: data plus results synthesize algorithms for next actions.".into(),
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
            title: "Heart Pass quota contract visible".into(),
            detail: format!(
                "Quota status is {}; {} of {} local pilot queries remain for period {}, with {} fallback.",
                heart_pass.quotaLayer.status,
                heart_pass.quotaLayer.remainingQueries,
                heart_pass.quotaLayer.includedQueries,
                heart_pass.quotaLayer.period,
                heart_pass.quotaLayer.fallback
            ),
            status: if heart_pass.quotaLayer.status == "planned" {
                "planned".into()
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
            scope: "ghost onboarding -> Brave key acquisition -> ignored solos/config/ghost.local.json".into(),
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
        AppEntry {
            name: "SolOS Pulso".into(),
            subtitle: "Planned social signal surface".into(),
            description: "Future consented social layer for posts, topics, video signals, and Pulso Credits with Wallet, Ghost, and Approvals mediation.".into(),
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
                    onboarding_url: read_onboarding_url().unwrap_or_else(|| {
                        "https://api-dashboard.search.brave.com/app/keys".into()
                    }),
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
                    onboarding_url: read_onboarding_url().unwrap_or_else(|| {
                        "https://api-dashboard.search.brave.com/app/keys".into()
                    }),
                    onboarding_status: "configured".into(),
                };
            }
        }
    }

    GhostIntelligenceState {
        available: false,
        source: "missing-config".into(),
        api_key_found: false,
        onboarding_url: read_onboarding_url()
            .unwrap_or_else(|| "https://api-dashboard.search.brave.com/app/keys".into()),
        onboarding_status: read_onboarding_status().unwrap_or_else(|| "needs-user-key".into()),
    }
}

fn brave_search(query: &str, source: &str) -> Result<Vec<GhostCitation>, String> {
    let api_key =
        resolve_brave_key().ok_or_else(|| format!("Brave API key unavailable from {}", source))?;
    let endpoint = format!(
        "https://api.search.brave.com/res/v1/web/search?q={}&count=5",
        url_encode(query)
    );
    let token_header = format!("X-Subscription-Token: {api_key}");
    let output = Command::new("curl")
        .args([
            "-fsSL",
            "--max-time",
            "12",
            "-H",
            "Accept: application/json",
            "-H",
            token_header.as_str(),
            endpoint.as_str(),
        ])
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

    let body: BraveSearchResponse =
        serde_json::from_slice(&output.stdout).map_err(|e| e.to_string())?;
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

        for key in [
            "BRAVE_API_KEY",
            "BRAVE_SEARCH_API_KEY",
            "SOLOS_BRAVE_API_KEY",
        ] {
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
        "./config/ghost.local.json",
        "../../config/ghost.local.json",
        "./solos/config/ghost.local.json",
        "./config/solos.json",
        "./config/runtime.json",
        "./.env",
        "./.env.local",
        "./app/shell/.env",
        "./app/shell/.env.local",
    ]
}

fn heart_pass_config_path() -> String {
    let candidates = [
        "../../config/heart_pass.json",
        "./config/heart_pass.json",
        "./solos/config/heart_pass.json",
    ];

    for candidate in candidates {
        if Path::new(candidate).exists() {
            return candidate.into();
        }
        if let Some(parent) = Path::new(candidate).parent() {
            if parent.exists() {
                return candidate.into();
            }
        }
    }

    "../../config/heart_pass.json".into()
}

fn default_heart_pass_config() -> HeartPassConfig {
    HeartPassConfig {
        schema: "solos.heart_pass.v1".into(),
        title: "SolOS Heart Pass".into(),
        network: "Polygon".into(),
        tokenStandard: "ERC-1155".into(),
        contract: "0x507783149b7abb6ce23414dd0c9742eb9f4549b4".into(),
        tokenId: "1".into(),
        openSeaUrl: "https://opensea.io/item/polygon/0x507783149b7abb6ce23414dd0c9742eb9f4549b4/1".into(),
        walletAddress: "".into(),
        ownerAddress: "".into(),
        verificationStatus: "needs-wallet".into(),
        lastCheckedAt: "never".into(),
        notes: "Local SolOS Heart Pass state. Wallet capture, Polygon verification, Ghost gating, and the planned quota contract stay visible before any sponsored backend is introduced.".into(),
        claimedPulsoRedemptions: vec![],
        quotaLayer: default_quota_layer_config(),
    }
}

fn default_quota_layer_config() -> QuotaLayerConfig {
    QuotaLayerConfig {
        status: "planned".into(),
        mode: "hybrid-sponsored-byok".into(),
        period: "local-pilot".into(),
        includedQueries: 25,
        usedQueries: 0,
        remainingQueries: 25,
        fallback: "byok".into(),
        usageSource: "not-active".into(),
        lastSync: "never".into(),
        resetPolicy: "manual until quota service exists".into(),
        notes: "Local placeholder for the Heart Pass Quota Layer. No sponsored provider key is used until a server-side quota service exists.".into(),
    }
}

fn merge_quota_layer_config(config: QuotaLayerConfig) -> QuotaLayerConfig {
    let default = default_quota_layer_config();
    let included = if config.includedQueries == 0 {
        default.includedQueries
    } else {
        config.includedQueries
    };
    let used = config.usedQueries.min(included);
    let remaining = included.saturating_sub(used);

    QuotaLayerConfig {
        status: if config.status.is_empty() {
            default.status
        } else {
            config.status
        },
        mode: if config.mode.is_empty() {
            default.mode
        } else {
            config.mode
        },
        period: if config.period.is_empty() {
            default.period
        } else {
            config.period
        },
        includedQueries: included,
        usedQueries: used,
        remainingQueries: remaining,
        fallback: if config.fallback.is_empty() {
            default.fallback
        } else {
            config.fallback
        },
        usageSource: if config.usageSource.is_empty() {
            default.usageSource
        } else {
            config.usageSource
        },
        lastSync: if config.lastSync.is_empty() {
            default.lastSync
        } else {
            config.lastSync
        },
        resetPolicy: if config.resetPolicy.is_empty() {
            default.resetPolicy
        } else {
            config.resetPolicy
        },
        notes: if config.notes.is_empty() {
            default.notes
        } else {
            config.notes
        },
    }
}

fn consume_ghost_queries(amount: u32) -> Result<QuotaConsumptionResult, String> {
    if amount == 0 || amount > 25 {
        return Err("query amount must be between 1 and 25".into());
    }

    let path = heart_pass_config_path();
    let mut config = load_heart_pass_config(&path);
    if config.verificationStatus != "verified-holder" {
        return Err("verified Heart Pass required before consuming sponsored quota".into());
    }

    config.quotaLayer = merge_quota_layer_config(config.quotaLayer);
    if config.quotaLayer.status != "active" {
        return Err("Ghost sponsored quota is not active".into());
    }
    if config.quotaLayer.remainingQueries < amount {
        return Err("insufficient Ghost query quota; use BYOK fallback".into());
    }

    config.quotaLayer.usedQueries = config.quotaLayer.usedQueries.saturating_add(amount);
    config.quotaLayer.remainingQueries = config
        .quotaLayer
        .includedQueries
        .saturating_sub(config.quotaLayer.usedQueries);
    config.quotaLayer.lastSync = unix_timestamp_string();
    config.quotaLayer.usageSource = "local-ghost-runtime".into();
    save_heart_pass_config(&path, &config)?;

    Ok(QuotaConsumptionResult {
        schema: "solos.ghost.quota-consumption.v1".into(),
        consumed: amount,
        usedQueries: config.quotaLayer.usedQueries,
        remainingQueries: config.quotaLayer.remainingQueries,
        status: if config.quotaLayer.remainingQueries == 0 {
            "exhausted".into()
        } else {
            "active".into()
        },
    })
}

fn load_heart_pass_config(path: &str) -> HeartPassConfig {
    if let Ok(content) = fs::read_to_string(path) {
        if let Ok(config) = serde_json::from_str::<HeartPassConfig>(&content) {
            return HeartPassConfig {
                schema: if config.schema.is_empty() {
                    "solos.heart_pass.v1".into()
                } else {
                    config.schema
                },
                title: if config.title.is_empty() {
                    "SolOS Heart Pass".into()
                } else {
                    config.title
                },
                network: if config.network.is_empty() {
                    "Polygon".into()
                } else {
                    config.network
                },
                tokenStandard: if config.tokenStandard.is_empty() {
                    "ERC-1155".into()
                } else {
                    config.tokenStandard
                },
                contract: if config.contract.is_empty() {
                    "0x507783149b7abb6ce23414dd0c9742eb9f4549b4".into()
                } else {
                    config.contract
                },
                tokenId: if config.tokenId.is_empty() {
                    "1".into()
                } else {
                    config.tokenId
                },
                openSeaUrl: if config.openSeaUrl.is_empty() {
                    "https://opensea.io/item/polygon/0x507783149b7abb6ce23414dd0c9742eb9f4549b4/1"
                        .into()
                } else {
                    config.openSeaUrl
                },
                walletAddress: config.walletAddress,
                ownerAddress: config.ownerAddress,
                verificationStatus: if config.verificationStatus.is_empty() {
                    "needs-wallet".into()
                } else {
                    config.verificationStatus
                },
                lastCheckedAt: if config.lastCheckedAt.is_empty() {
                    "never".into()
                } else {
                    config.lastCheckedAt
                },
                notes: if config.notes.trim().is_empty()
                    || config
                        .notes
                        .contains("Polygon ownership verification is a later stage")
                {
                    "Local SolOS Heart Pass state. Wallet capture, Polygon verification, Ghost gating, and the planned quota contract stay visible before any sponsored backend is introduced.".into()
                } else {
                    config.notes
                },
                claimedPulsoRedemptions: config.claimedPulsoRedemptions,
                quotaLayer: merge_quota_layer_config(config.quotaLayer),
            };
        }
    }

    default_heart_pass_config()
}

fn ensure_heart_pass_config(path: &str, config: &HeartPassConfig) {
    if Path::new(path).exists() {
        return;
    }

    let _ = save_heart_pass_config(path, config);
}

fn save_heart_pass_config(path: &str, config: &HeartPassConfig) -> Result<(), String> {
    if let Some(parent) = Path::new(path).parent() {
        fs::create_dir_all(parent).map_err(|error| error.to_string())?;
    }
    let payload = serde_json::to_string_pretty(config).map_err(|error| error.to_string())?;
    let temporary_path = format!("{path}.tmp");
    fs::write(&temporary_path, payload).map_err(|error| error.to_string())?;
    fs::rename(&temporary_path, path).map_err(|error| error.to_string())
}

fn build_heart_pass_state() -> HeartPassState {
    let config_path = heart_pass_config_path();
    let config = load_heart_pass_config(&config_path);
    ensure_heart_pass_config(&config_path, &config);

    let has_wallet = !config.walletAddress.trim().is_empty();
    let quota_layer = build_quota_layer_state(&config.quotaLayer, &config.verificationStatus);
    let status = match config.verificationStatus.as_str() {
        "verified-holder" => "verified holder · quota visible".into(),
        "not-holder" => "wallet checked · pass not found".into(),
        "verification-error" => "wallet checked · verification error".into(),
        "wallet-configured-unverified" => {
            "local wallet captured · needs Polygon verification".into()
        }
        _ if has_wallet => format!("local wallet captured · {}", config.verificationStatus),
        _ => "visible · needs wallet address".into(),
    };
    let next_step = match config.verificationStatus.as_str() {
        "verified-holder" => {
            "Use the visible quota contract in Ghost research UX, then design the server-side quota service before sponsoring real provider calls.".into()
        }
        "not-holder" => {
            "Use a wallet that holds the Heart Pass or keep Ghost research on BYOK without sponsored quota.".into()
        }
        "verification-error" => {
            "Retry Polygon verification and keep BYOK as the fallback until holder state is clear.".into()
        }
        _ if has_wallet => {
            "Verify the configured wallet against Polygon balanceOf for Anastacia Our Hearts #1.".into()
        }
        _ => "Add a Polygon wallet address locally, then verify Heart Pass ownership.".into(),
    };
    let summary = match config.verificationStatus.as_str() {
        "verified-holder" => "Heart Pass ownership is verified in the local runtime contract. Ghost onboarding and the planned quota layer can now explain holder utility without hiding cost or provider ownership.".into(),
        _ => "Heart Pass state is part of the SolOS runtime contract. Wallet capture, Polygon verification, Ghost gating, and quota visibility stay explicit before any sponsored backend is introduced.".into(),
    };

    HeartPassState {
        title: config.title,
        status,
        network: config.network,
        tokenStandard: config.tokenStandard,
        contract: config.contract,
        tokenId: config.tokenId,
        openSeaUrl: config.openSeaUrl,
        summary,
        nextStep: next_step,
        walletAddress: if has_wallet {
            config.walletAddress
        } else {
            "not configured".into()
        },
        ownerAddress: if config.ownerAddress.trim().is_empty() {
            "not checked".into()
        } else {
            config.ownerAddress
        },
        verificationStatus: config.verificationStatus,
        lastCheckedAt: config.lastCheckedAt,
        configPath: config_path,
        pulsoRewardsClaimed: config.claimedPulsoRedemptions.len(),
        quotaLayer: quota_layer,
        capabilities: vec![
            "Early-supporter identity surface".into(),
            "Guided Ghost/Brave key onboarding eligibility".into(),
            format!(
                "Quota Layer: {} local pilot queries, {} fallback, status {}",
                config.quotaLayer.includedQueries,
                config.quotaLayer.fallback,
                config.quotaLayer.status
            ),
            format!(
                "Pulso Founder rewards synchronized: {}",
                config.claimedPulsoRedemptions.len()
            ),
            "Future pass-gated experimental Ghost capabilities".into(),
            "Transparent usage and quota story without investment/yield promises".into(),
        ],
    }
}

fn build_quota_layer_state(
    config: &QuotaLayerConfig,
    verification_status: &str,
) -> QuotaLayerState {
    let included = config.includedQueries;
    let used = config.usedQueries.min(included);
    let remaining = included.saturating_sub(used);
    let verified = verification_status == "verified-holder";
    let status = if verified {
        config.status.clone()
    } else {
        "verification-required".into()
    };
    let usage_source = if verified {
        config.usageSource.clone()
    } else {
        "waiting-for-pass-verification".into()
    };
    let summary = if verified {
        format!(
            "The Heart Pass Quota Layer is visible as a local runtime contract: {} included Ghost research queries for {}, {} used, {} remaining. Sponsored calls are still planned; BYOK remains the active fallback.",
            included, config.period, used, remaining
        )
    } else {
        format!(
            "The Quota Layer is defined but disabled until Heart Pass ownership is verified. The planned local allowance is {} Ghost research queries with {} fallback.",
            included, config.fallback
        )
    };
    let next_step = if verified {
        "Keep the local quota visible in Wallet and Agent/Ghost, then add a signed server-side quota service before any sponsored Brave/OpenAI usage.".into()
    } else {
        "Verify the Heart Pass in Wallet Hub before treating quota as holder utility. Until then, Ghost research stays BYOK-only.".into()
    };

    QuotaLayerState {
        title: "Heart Pass Quota Layer".into(),
        status,
        mode: config.mode.clone(),
        period: config.period.clone(),
        includedQueries: included,
        usedQueries: used,
        remainingQueries: remaining,
        fallback: config.fallback.clone(),
        usageSource: usage_source,
        lastSync: config.lastSync.clone(),
        resetPolicy: config.resetPolicy.clone(),
        summary,
        nextStep: next_step,
    }
}

fn build_capability_manifest() -> CapabilityManifest {
    CapabilityManifest {
        schema: "solos.capabilities.v1".into(),
        defaultPolicy: "deny".into(),
        capabilities: vec![
            CapabilityDefinition {
                id: "app.open.safe".into(),
                scopes: vec!["read".into(), "local".into()],
                risk: "low".into(),
                approval: "once-per-request".into(),
                executable: true,
                audit: "required".into(),
            },
            CapabilityDefinition {
                id: "web.search.read".into(),
                scopes: vec!["read".into(), "network".into(), "billable".into()],
                risk: "medium".into(),
                approval: "required-for-sponsored-or-account-linked-use".into(),
                executable: false,
                audit: "required".into(),
            },
            CapabilityDefinition {
                id: "filesystem.write".into(),
                scopes: vec!["write".into(), "sensitive".into()],
                risk: "high".into(),
                approval: "required".into(),
                executable: false,
                audit: "required".into(),
            },
            CapabilityDefinition {
                id: "wallet.sign".into(),
                scopes: vec!["wallet".into(), "sensitive".into()],
                risk: "critical".into(),
                approval: "explicit-every-time".into(),
                executable: false,
                audit: "required".into(),
            },
            CapabilityDefinition {
                id: "public.post".into(),
                scopes: vec!["write".into(), "network".into(), "public".into()],
                risk: "high".into(),
                approval: "explicit-every-time".into(),
                executable: false,
                audit: "required".into(),
            },
        ],
    }
}

fn trace_store_path() -> String {
    env::var("SOLOS_TRACE_STORE").unwrap_or_else(|_| "../../data/ghost-traces.json".into())
}

fn evaluation_seed() -> Vec<TraceRecord> {
    vec![
        TraceRecord {
            traceId: "eval-docs-local".into(),
            requestClass: "documentation task".into(),
            selectedRoute: "local-docs".into(),
            expectedRoute: "local-docs".into(),
            outcome: "accepted".into(),
            approvalRequired: false,
            quotaCost: 0,
            correctedRoute: None,
        },
        TraceRecord {
            traceId: "eval-safe-app-open".into(),
            requestClass: "system action request".into(),
            selectedRoute: "approval-then-app.open.safe".into(),
            expectedRoute: "approval-then-app.open.safe".into(),
            outcome: "accepted".into(),
            approvalRequired: true,
            quotaCost: 0,
            correctedRoute: None,
        },
        TraceRecord {
            traceId: "eval-public-post-deny".into(),
            requestClass: "content/publishing task".into(),
            selectedRoute: "default-deny-until-explicit-approval".into(),
            expectedRoute: "default-deny-until-explicit-approval".into(),
            outcome: "accepted".into(),
            approvalRequired: true,
            quotaCost: 0,
            correctedRoute: None,
        },
        TraceRecord {
            traceId: "eval-wallet-sign-deny".into(),
            requestClass: "wallet/pass request".into(),
            selectedRoute: "explicit-wallet-approval".into(),
            expectedRoute: "explicit-wallet-approval".into(),
            outcome: "accepted".into(),
            approvalRequired: true,
            quotaCost: 0,
            correctedRoute: None,
        },
        TraceRecord {
            traceId: "eval-unclear-clarify".into(),
            requestClass: "unclear request".into(),
            selectedRoute: "clarify-first".into(),
            expectedRoute: "clarify-first".into(),
            outcome: "accepted".into(),
            approvalRequired: false,
            quotaCost: 0,
            correctedRoute: None,
        },
    ]
}

fn load_trace_evaluation() -> TraceEvaluation {
    let path = trace_store_path();
    let records = fs::read_to_string(&path)
        .ok()
        .and_then(|payload| serde_json::from_str::<Vec<TraceRecord>>(&payload).ok())
        .filter(|items| !items.is_empty())
        .unwrap_or_else(evaluation_seed);
    if !Path::new(&path).exists() {
        if let Some(parent) = Path::new(&path).parent() {
            let _ = fs::create_dir_all(parent);
        }
        if let Ok(payload) = serde_json::to_string_pretty(&records) {
            let _ = fs::write(&path, payload);
        }
    }
    let accepted = records
        .iter()
        .filter(|item| item.outcome == "accepted")
        .count();
    let rejected = records
        .iter()
        .filter(|item| item.outcome == "rejected")
        .count();
    let corrected = records
        .iter()
        .filter(|item| item.outcome == "corrected")
        .count();
    let pass_rate = if records.is_empty() {
        0.0
    } else {
        accepted as f64 / records.len() as f64
    };
    TraceEvaluation {
        schema: "solos.ghost.trace-evaluation.v1".into(),
        storePath: path,
        total: records.len(),
        accepted,
        rejected,
        corrected,
        passRate: pass_rate,
        records,
    }
}

fn build_mediated_action() -> MediatedAction {
    MediatedAction {
        id: "demo-open-workspace".into(),
        capability: "app.open.safe".into(),
        target: "workspace".into(),
        status: "approval-ready".into(),
        approvalRequired: true,
        result: "The web shell executes only after visible approval; native runtime exports the same boundary without spawning arbitrary commands.".into(),
    }
}

fn build_wallet_session(heart_pass: &HeartPassState) -> WalletSession {
    let verified = heart_pass.verificationStatus == "verified-holder";
    WalletSession {
        status: if verified {
            "holder-verified"
        } else {
            "local-unverified"
        }
        .into(),
        proofMode: if verified {
            "erc1155-balance-observed; signature-required-for-sponsored-call"
        } else {
            "none"
        }
        .into(),
        addressPersistence: "repository-local; revocable by clearing config".into(),
        sponsoredCallsAllowed: false,
        failureReason: if verified {
            "signed session proof service is not configured".into()
        } else {
            format!("Heart Pass status is {}", heart_pass.verificationStatus)
        },
    }
}

fn build_memory_policy() -> MemoryPolicy {
    MemoryPolicy {
        defaultRetention: "session-only".into(),
        classes: vec![
            MemoryClass {
                name: "session".into(),
                retention: "until-session-end".into(),
                revocation: "automatic".into(),
                sensitive: false,
            },
            MemoryClass {
                name: "project".into(),
                retention: "repository-controlled".into(),
                revocation: "delete-or-revert".into(),
                sensitive: false,
            },
            MemoryClass {
                name: "preference".into(),
                retention: "explicit-opt-in".into(),
                revocation: "user-delete".into(),
                sensitive: false,
            },
            MemoryClass {
                name: "sensitive".into(),
                retention: "default-deny".into(),
                revocation: "immediate-delete".into(),
                sensitive: true,
            },
        ],
    }
}

fn build_quota_proxy_contract() -> QuotaProxyContract {
    QuotaProxyContract {
        endpoint: "POST /v1/ghost/research".into(),
        status: "contract-ready-backend-not-configured".into(),
        requiresSignedHolderProof: true,
        idempotencyRequired: true,
        providerNeutral: true,
        fallback: "BYOK".into(),
    }
}

struct GhostKnowledgeSeed {
    id: &'static str,
    name: &'static str,
    query: String,
    summary: &'static str,
}

fn ghost_knowledge_curriculum() -> Vec<GhostKnowledgeSeed> {
    vec![
        GhostKnowledgeSeed {
            id: "natural-language-command-understanding",
            name: "Natural language command understanding",
            query: "natural language interface intent recognition command understanding best practices".into(),
            summary: "How Ghost should translate user language into structured SolOS intents without over-executing vague requests.",
        },
        GhostKnowledgeSeed {
            id: "rag-grounding-and-citations",
            name: "RAG grounding and citations",
            query: "retrieval augmented generation grounding citations knowledge base best practices".into(),
            summary: "How Ghost should use retrieved evidence, source snippets, and citations before answering with external claims.",
        },
        GhostKnowledgeSeed {
            id: "agent-tool-safety-approval-flow",
            name: "Agent tool safety and approval flow",
            query: "AI agent tool use safety approval workflow human in the loop best practices".into(),
            summary: "How Ghost should separate harmless answers from sensitive actions that require explicit user approval.",
        },
        GhostKnowledgeSeed {
            id: "local-first-personal-ai-memory",
            name: "Local-first personal AI memory",
            query: "local first personal AI assistant memory privacy knowledge base design".into(),
            summary: "How Ghost should keep useful user/system context locally while avoiding unsafe or unnecessary data collection.",
        },
        GhostKnowledgeSeed {
            id: "operating-system-assistant-ux",
            name: "Operating-system assistant UX",
            query: "operating system AI assistant user experience proactive contextual help natural language".into(),
            summary: "How Ghost should feel useful inside SolOS: contextual, concise, interrupting only when there is value.",
        },
        GhostKnowledgeSeed {
            id: "planning-task-decomposition",
            name: "Planning and task decomposition",
            query: "AI agent planning task decomposition execution monitoring best practices".into(),
            summary: "How Ghost should break bigger goals into safe steps, monitor progress, and report useful outcomes.",
        },
    ]
}

fn ghost_knowledge_database_path() -> String {
    let candidates = [
        "../../config/ghost_knowledge.json",
        "./config/ghost_knowledge.json",
        "./solos/config/ghost_knowledge.json",
    ];

    for candidate in candidates {
        if let Some(parent) = Path::new(candidate).parent() {
            if parent.exists() {
                return candidate.into();
            }
        }
    }

    "../../config/ghost_knowledge.json".into()
}

fn load_ghost_knowledge_database(path: &str) -> GhostKnowledgeDatabase {
    if let Ok(content) = fs::read_to_string(path) {
        if let Ok(database) = serde_json::from_str::<GhostKnowledgeDatabase>(&content) {
            return database;
        }
    }

    let now = unix_timestamp_string();
    GhostKnowledgeDatabase {
        schema: "solos.ghost.knowledge.v1".into(),
        purpose: "Repo-local initiation cache for Ghost: natural-language use, grounded answers, approval-aware agent behavior, and SolOS UX.".into(),
        createdAt: now.clone(),
        updatedAt: now,
        topics: vec![],
    }
}

fn save_ghost_knowledge_database(
    path: &str,
    database: &GhostKnowledgeDatabase,
) -> Result<(), String> {
    if let Some(parent) = Path::new(path).parent() {
        fs::create_dir_all(parent).map_err(|e| e.to_string())?;
    }

    let payload = serde_json::to_string_pretty(database).map_err(|e| e.to_string())?;
    fs::write(path, payload).map_err(|e| e.to_string())
}

fn unix_timestamp_string() -> String {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .map(|duration| duration.as_secs().to_string())
        .unwrap_or_else(|_| "0".into())
}

fn read_onboarding_url() -> Option<String> {
    read_ghost_template_value("/ghost/intelligence/webSearch/onboardingUrl")
}

fn read_onboarding_status() -> Option<String> {
    read_ghost_template_value("/ghost/intelligence/webSearch/status")
}

fn read_ghost_template_value(pointer: &str) -> Option<String> {
    [
        "./config/ghost.json",
        "../../config/ghost.json",
        "./solos/config/ghost.json",
    ]
    .iter()
    .find_map(|path| fs::read_to_string(path).ok())
    .and_then(|content| serde_json::from_str::<serde_json::Value>(&content).ok())?
    .pointer(pointer)
    .and_then(|v| v.as_str())
    .map(|v| v.to_string())
}

fn url_encode(value: &str) -> String {
    let mut encoded = String::new();
    for byte in value.bytes() {
        match byte {
            b'A'..=b'Z' | b'a'..=b'z' | b'0'..=b'9' | b'-' | b'_' | b'.' | b'~' => {
                encoded.push(byte as char)
            }
            b' ' => encoded.push_str("%20"),
            _ => encoded.push_str(&format!("%{:02X}", byte)),
        }
    }
    encoded
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn support_contracts_are_safe_by_default() {
        let manifest = build_capability_manifest();
        assert_eq!(manifest.defaultPolicy, "deny");
        assert!(manifest
            .capabilities
            .iter()
            .any(|item| item.id == "wallet.sign"));
        assert!(build_quota_proxy_contract().requiresSignedHolderProof);
    }

    #[test]
    fn evaluation_seed_covers_core_safety_routes() {
        let records = evaluation_seed();
        assert!(records.len() >= 5);
        assert!(records
            .iter()
            .all(|item| item.selectedRoute == item.expectedRoute));
        assert!(records
            .iter()
            .any(|item| item.requestClass == "unclear request"));
        assert!(records
            .iter()
            .any(|item| item.requestClass == "wallet/pass request"));
    }

    #[test]
    fn quota_never_exceeds_included_allowance() {
        let config = QuotaLayerConfig {
            status: "active".into(),
            includedQueries: 3,
            usedQueries: 99,
            ..Default::default()
        };
        let state = build_quota_layer_state(&config, "verified-holder");
        assert_eq!(state.usedQueries, 3);
        assert_eq!(state.remainingQueries, 0);
    }
}
