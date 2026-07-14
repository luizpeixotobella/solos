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
  version: "v0.1-foundation",
  syncState: "Synced",
  notificationsCount: 2,
  approvalsCount: 1,
};

export const initialAgentState: AgentState = {
  status: "awaiting-approval",
  currentTask: "Resume Workspace",
  recentActions: ["Workspace resume prepared", "Wallet summary updated", "Forum launch pack prepared"],
  pendingApprovals: 1,
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
    detail: "Workspace can be resumed after approval review.",
    status: "awaiting-approval",
  },
  {
    id: "task-forum-pack",
    title: "Forum launch pack completed",
    detail: "Narrative and launch materials remain available for reuse.",
    status: "done",
  },
];

export const initialApprovals: ApprovalItem[] = [
  {
    id: "approval-workspace",
    title: "Resume workspace session",
    description: "Open the workspace module and restore recent session context.",
    impact: "Non-sensitive system surface action. No wallet signing required.",
    status: "pending",
  },
];

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
