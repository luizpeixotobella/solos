export type ScreenKey = "home" | "agent" | "wallet" | "apps";

export type AgentStatus =
  | "idle"
  | "listening"
  | "thinking"
  | "proposing"
  | "awaiting-approval"
  | "executing"
  | "completed"
  | "failed";

export type WalletStatus = "disconnected" | "connecting" | "connected" | "signature-pending" | "failed";

export type AppKind = "local" | "web" | "dapp" | "hybrid";

export type UserSession = {
  id: string;
  displayName: string;
  environmentMode: string;
  lastActiveAt: string;
};

export type SystemStatus = {
  online: boolean;
  version: string;
  syncState: string;
  notificationsCount: number;
  approvalsCount: number;
};

export type AgentState = {
  status: AgentStatus;
  currentTask: string | null;
  recentActions: string[];
  pendingApprovals: number;
};

export type AgentConversationMessage = {
  id: string;
  role: "user" | "agent" | "system";
  text: string;
  tone?: "default" | "accent";
};

export type TaskItem = {
  id: string;
  title: string;
  detail: string;
  status: "queued" | "active" | "awaiting-approval" | "done" | "failed";
};

export type ApprovalItem = {
  id: string;
  title: string;
  description: string;
  impact: string;
  status: "pending" | "approved" | "denied";
};

export type WalletAsset = {
  id: string;
  name: string;
  symbol: string;
  amount: string;
};

export type WalletState = {
  status: WalletStatus;
  address: string;
  network: string;
  balances: { label: string; value: string }[];
  assets: WalletAsset[];
};

export type AppDefinition = {
  id: string;
  name: string;
  kind: AppKind;
  description: string;
  capabilities: string[];
  status: "available" | "pinned" | "active";
};

export type DemoStep = {
  id: string;
  title: string;
  summary: string;
  target: ScreenKey;
  status: "upcoming" | "current" | "completed";
};

export type SolOSState = {
  session: UserSession;
  system: SystemStatus;
  agent: AgentState;
  wallet: WalletState;
  apps: AppDefinition[];
  tasks: TaskItem[];
  approvals: ApprovalItem[];
  conversation: AgentConversationMessage[];
  demoFlow: DemoStep[];
};
