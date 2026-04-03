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
