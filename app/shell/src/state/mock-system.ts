import type {
  AgentConversationMessage,
  AgentState,
  AppDefinition,
  ApprovalItem,
  ScreenKey,
  SystemStatus,
  TaskItem,
  UserSession,
  WalletState,
} from "../types/system";

export const defaultScreen: ScreenKey = "home";

export const initialSession: UserSession = {
  id: "luiz-main",
  displayName: "Luiz",
  environmentMode: "SolOS Environment Active",
  lastActiveAt: new Date().toISOString(),
};

export const initialSystemStatus: SystemStatus = {
  online: true,
  version: "v1.0.0-rc2",
  syncState: "Synced",
  notificationsCount: 2,
  approvalsCount: 0,
};

export const initialAgentState: AgentState = {
  status: "proposing",
  currentTask: "Resume Workspace",
  recentActions: ["Workspace resume prepared", "Wallet summary updated", "Forum launch pack prepared"],
  pendingApprovals: 0,
  requestClassifier: {
    title: "Ghost request classifier",
    summary:
      "First W3Schools-inspired router: Ghost classifies user language before choosing a route, asking approval, or spending quota.",
    exampleRequest: "Turn the archived W3Schools AI material into the next AI Ghost implementation slice.",
    classes: [
      {
        name: "research request",
        status: "byok-or-local-only",
        safetyLevel: "low-to-medium",
        requiredTools: "local cache, optional web.search.read",
        approvalNeeds: "approval when the route consumes paid/sponsored quota or opens account-linked setup",
        quotaCost: "0 local queries now; BYOK required for web until holder quota is active",
        route: "answer from local docs first; use grounded retrieval only when configured and justified",
      },
      {
        name: "system action request",
        status: "approval-gated",
        safetyLevel: "high",
        requiredTools: "task.intent.dispatch, filesystem or command capability after policy review",
        approvalNeeds: "explicit approval before shell commands, host changes, wallet actions, or app launches",
        quotaCost: "0 model/web quota by itself; execution trace must still be recorded",
        route: "classify, explain, request approval, then execute through a mediated task boundary",
      },
      {
        name: "documentation task",
        status: "ready",
        safetyLevel: "low",
        requiredTools: "repo docs and structured runtime state",
        approvalNeeds: "no approval for local docs; approval required before public publishing",
        quotaCost: "0",
        route: "update docs alongside code so product doctrine and implementation do not drift",
      },
      {
        name: "risky external action",
        status: "default-deny",
        safetyLevel: "critical",
        requiredTools: "network, account, payment, public-send, or destructive host capability",
        approvalNeeds: "must be stopped or escalated before execution",
        quotaCost: "not relevant until safety route is accepted",
        route: "refuse unsafe requests or move into a narrow approval lane with full impact text",
      },
    ],
  },
  actionTrace: {
    traceId: "ghost-trace-local-archive",
    request: "Turn the archived W3Schools AI material into the next AI Ghost implementation slice.",
    data: "W3Schools AI archive, SolOS runtime snapshot, Heart Pass quota contract, Ghost readiness doctrine, and local repository state.",
    resultTarget: "Visible Ghost classifier, action trace, and route explanation in Agent/Ghost without public posting.",
    algorithmRoute: "classify request -> score safety/quota/approval needs -> choose local implementation route -> document outcome",
    outcome: "local-implementation-prepared",
    quotaCost: "0 holder queries; the archive already exists and no fresh web research is consumed.",
    approvalRequired:
      "No external approval for local repo work; explicit approval remains required before public posting, wallet signing, paid provider use, or destructive host actions.",
  },
  routeExplanation: {
    selectedClass: "documentation task + system action request",
    selectedRoute: "local-implementation-from-archived-evidence",
    safetyLevel: "medium",
    explanation:
      "The request continues product implementation from an archived learning source. The safe route is local code/docs work with a visible trace, not fresh web spending or public publishing.",
    approvalPolicy:
      "Repo edits and local builds can proceed in the development workspace. External messages, posts, wallet signatures, paid-provider calls, and destructive commands stay approval-gated.",
    quotaPolicy: "This route spends 0 research queries; future web-grounded research should decrement Heart Pass quota.",
    nextStep: "Persist trace outcomes and accepted/rejected examples so Ghost can compare future routes against expected behavior.",
  },
  resolutionLoop: {
    schema: "solos.ghost.resolutions.v1",
    selectedId: "resolution-safe-workspace",
    summary:
      "One bounded objective is selected. Ghost can turn it into a visible plan, ask for approval, execute one mediated capability, and retain evidence of the result.",
    resolutions: [
      {
        id: "resolution-safe-workspace",
        title: "Restore my workspace, safely",
        objective: "Resume the Workspace module without bypassing the SolOS approval boundary.",
        targetOutcome:
          "Workspace is active, the user approval is recorded, and the app.open.safe result is attached as evidence.",
        status: "selected",
        readiness: "ready",
        progress: 20,
        currentStep: "Build a bounded plan",
        capability: "app.open.safe",
        resultSummary: "Objective selected; execution has not started.",
        steps: [
          { id: "understand", title: "Understand the objective", status: "completed", capability: "task.intent.classify", result: "Bounded local app action." },
          { id: "plan", title: "Build a bounded plan", status: "active", capability: "task.plan.local", result: "" },
          { id: "approval", title: "Ask for explicit approval", status: "pending", capability: "approval.request", result: "" },
          { id: "execute", title: "Execute app.open.safe", status: "pending", capability: "app.open.safe", result: "" },
          { id: "verify", title: "Verify the target outcome", status: "pending", capability: "app.state.read", result: "" },
        ],
        evidence: ["Objective selected"],
      },
      {
        id: "resolution-grounded-answer",
        title: "Research a grounded answer",
        objective: "Research a fresh question and return source-linked evidence.",
        targetOutcome: "Answer, citations, quota receipt, and trace are retained together.",
        status: "candidate",
        readiness: "needs-quota-or-byok",
        progress: 0,
        currentStep: "Waiting for a configured research route",
        capability: "web.search.read",
        resultSummary: "Not executable until a quota or BYOK route is available.",
        steps: [{ id: "research-route", title: "Configure research route", status: "blocked", capability: "web.search.read", result: "Heart Pass quota or BYOK is required." }],
        evidence: [],
      },
      {
        id: "resolution-public-launch",
        title: "Prepare and publish a launch",
        objective: "Turn a completed product change into a reviewed multi-channel launch.",
        targetOutcome: "Verified public URLs are attached to the same resolution trace.",
        status: "candidate",
        readiness: "needs-public-send-adapter",
        progress: 0,
        currentStep: "Waiting for an account-bound publish adapter",
        capability: "public.post.create",
        resultSummary: "Not executable inside SolOS until a public-send adapter exists.",
        steps: [{ id: "publish-adapter", title: "Connect publish adapter", status: "blocked", capability: "public.post.create", result: "Public posting is outside the current capability manifest." }],
        evidence: [],
      },
    ],
  },
  evaluation: {
    total: 5,
    accepted: 5,
    rejected: 0,
    corrected: 0,
    passRate: 1,
  },
};

export const initialWalletState: WalletState = {
  status: "connected",
  address: "9xLu...Ghost",
  network: "Solana",
  balances: [
    { label: "SOL", value: "12.84" },
    { label: "USDC", value: "248.00" },
  ],
  assets: [
    { id: "asset-1", name: "SolOS Genesis Pass", symbol: "SGP", amount: "1" },
    { id: "asset-2", name: "USD Coin", symbol: "USDC", amount: "248.00" },
  ],
};

export const initialApps: AppDefinition[] = [
  {
    id: "workspace",
    name: "Workspace",
    kind: "local",
    description: "Core working environment and recent sessions.",
    capabilities: ["system", "agent"],
    status: "pinned",
  },
  {
    id: "wallet-hub",
    name: "Wallet Hub",
    kind: "dapp",
    description: "Asset visibility, signatures, and account activity.",
    capabilities: ["wallet", "identity"],
    status: "available",
  },
  {
    id: "notes-mesh",
    name: "Notes Mesh",
    kind: "hybrid",
    description: "Notes and memory flows across local and shared contexts.",
    capabilities: ["storage", "agent"],
    status: "available",
  },
  {
    id: "solos-pulso",
    name: "SolOS Pulso",
    kind: "hybrid",
    description: "Planned social signal surface for posts, topics, video signals, and Pulso Credits.",
    capabilities: ["social", "agent", "wallet", "approvals"],
    status: "available",
  },
];

export const initialTasks: TaskItem[] = [
  {
    id: "task-wallet",
    title: "Wallet summary updated",
    detail: "Balances and asset visibility were synchronized for the current session.",
    status: "done",
  },
  {
    id: "task-workspace",
    title: "Workspace resume prepared",
    detail: "Ghost selected this objective; build the bounded plan to open approval.",
    status: "queued",
  },
  {
    id: "task-forum-pack",
    title: "Forum launch pack completed",
    detail: "Narrative and launch materials remain available for reuse.",
    status: "done",
  },
];

export const initialApprovals: ApprovalItem[] = [];

export const initialAgentConversation: AgentConversationMessage[] = [
  {
    id: "msg-user-1",
    role: "user",
    text: "Show my wallet and open my workspace.",
  },
  {
    id: "msg-agent-1",
    role: "agent",
    text: "Wallet summary is ready. Workspace can be opened after approval review.",
    tone: "accent",
  },
];
