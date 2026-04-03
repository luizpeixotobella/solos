import { DemoFlowPanel } from "./demo-flow-panel";
import { PanelCard } from "./panel-card";
import { StatusBadge } from "./status-badge";
import { colors } from "../styles/tokens";
import type { ApprovalItem, ScreenKey, SolOSState, TaskItem } from "../types/system";

function EnvironmentPanel({ state }: { state: SolOSState }) {
  return (
    <PanelCard title="Environment">
      <div style={{ display: "grid", gap: 10, color: colors.soft }}>
        <div style={{ display: "flex", justifyContent: "space-between", gap: 12 }}>
          <span>Session</span>
          <strong>{state.session.displayName}</strong>
        </div>
        <div style={{ display: "flex", justifyContent: "space-between", gap: 12 }}>
          <span>System</span>
          <StatusBadge label={state.system.syncState} tone="success" />
        </div>
        <div style={{ display: "flex", justifyContent: "space-between", gap: 12 }}>
          <span>Agent</span>
          <StatusBadge label={state.agent.status} tone="accent" />
        </div>
      </div>
    </PanelCard>
  );
}

function FocusTaskPanel({ activeTask }: { activeTask: TaskItem | null }) {
  return (
    <PanelCard title="Focus Task" aside={activeTask ? <StatusBadge label={activeTask.status.replace("-", " ")} tone="accent" /> : undefined}>
      {activeTask ? (
        <div style={{ display: "grid", gap: 8 }}>
          <strong>{activeTask.title}</strong>
          <div style={{ color: colors.soft }}>{activeTask.detail}</div>
        </div>
      ) : (
        <div style={{ color: colors.soft }}>No active flow is competing for attention.</div>
      )}
    </PanelCard>
  );
}

function ApprovalsPanel({ pendingApprovals }: { pendingApprovals: ApprovalItem[] }) {
  return (
    <PanelCard title="Approvals" aside={<StatusBadge label={`${pendingApprovals.length} pending`} tone={pendingApprovals.length > 0 ? "accent" : "success"} />}>
      {pendingApprovals.length > 0 ? (
        <div style={{ display: "grid", gap: 10 }}>
          {pendingApprovals.map((approval) => (
            <div key={approval.id} style={{ padding: 12, borderRadius: 12, background: "rgba(255,255,255,0.04)" }}>
              <strong>{approval.title}</strong>
              <div style={{ color: colors.muted, marginTop: 6 }}>{approval.impact}</div>
            </div>
          ))}
        </div>
      ) : (
        <div style={{ color: colors.soft }}>No pending approvals right now.</div>
      )}
    </PanelCard>
  );
}

function RecentActionsPanel({ state }: { state: SolOSState }) {
  return (
    <PanelCard title="Recent Agent Actions">
      <ul style={{ margin: 0, paddingLeft: 18, color: colors.soft, display: "grid", gap: 8 }}>
        {state.agent.recentActions.slice(0, 4).map((item) => (
          <li key={item}>{item}</li>
        ))}
      </ul>
    </PanelCard>
  );
}

function WalletRail({ state, pendingApprovals }: { state: SolOSState; pendingApprovals: ApprovalItem[] }) {
  return (
    <>
      <PanelCard title="Wallet Status" aside={<StatusBadge label={state.wallet.status} tone="success" />}>
        <div style={{ display: "grid", gap: 8, color: colors.soft }}>
          <div>Network: {state.wallet.network}</div>
          <div>Address: {state.wallet.address}</div>
          <div>Assets: {state.wallet.assets.length}</div>
        </div>
      </PanelCard>
      <PanelCard title="Balances">
        <div style={{ display: "grid", gap: 10 }}>
          {state.wallet.balances.map((balance) => (
            <div key={balance.label} style={{ display: "flex", justifyContent: "space-between", gap: 12, color: colors.soft }}>
              <span>{balance.label}</span>
              <strong>{balance.value}</strong>
            </div>
          ))}
        </div>
      </PanelCard>
      <ApprovalsPanel pendingApprovals={pendingApprovals} />
    </>
  );
}

function AppsRail({ state }: { state: SolOSState }) {
  const activeApps = state.apps.filter((app) => app.status === "active");
  const pinnedApps = state.apps.filter((app) => app.status === "pinned" || app.status === "active");

  return (
    <>
      <PanelCard title="Runtime Status">
        <div style={{ display: "grid", gap: 8, color: colors.soft }}>
          <div>Total modules: {state.apps.length}</div>
          <div>Active modules: {activeApps.length}</div>
          <div>Pinned modules: {pinnedApps.length}</div>
        </div>
      </PanelCard>
      <PanelCard title="Pinned Modules">
        <ul style={{ margin: 0, paddingLeft: 18, color: colors.soft, display: "grid", gap: 8 }}>
          {pinnedApps.map((app) => (
            <li key={app.id}>
              {app.name} · {app.status}
            </li>
          ))}
        </ul>
      </PanelCard>
      <RecentActionsPanel state={state} />
      <DemoFlowPanel steps={state.demoFlow} compact />
    </>
  );
}

function HomeRail({ state, activeTask, pendingApprovals }: { state: SolOSState; activeTask: TaskItem | null; pendingApprovals: ApprovalItem[] }) {
  return (
    <>
      <EnvironmentPanel state={state} />
      <FocusTaskPanel activeTask={activeTask} />
      <ApprovalsPanel pendingApprovals={pendingApprovals} />
      <RecentActionsPanel state={state} />
    </>
  );
}

function AgentRail({ state, activeTask, pendingApprovals }: { state: SolOSState; activeTask: TaskItem | null; pendingApprovals: ApprovalItem[] }) {
  return (
    <>
      <FocusTaskPanel activeTask={activeTask} />
      <ApprovalsPanel pendingApprovals={pendingApprovals} />
      <PanelCard title="Conversation State">
        <div style={{ display: "grid", gap: 8, color: colors.soft }}>
          <div>Messages: {state.conversation.length}</div>
          <div>Current task: {state.agent.currentTask ?? "None"}</div>
          <div>Pending approvals: {state.agent.pendingApprovals}</div>
        </div>
      </PanelCard>
      <RecentActionsPanel state={state} />
    </>
  );
}

export function ContextRail({
  screen,
  state,
  activeTask,
  pendingApprovals,
}: {
  screen: ScreenKey;
  state: SolOSState;
  activeTask: TaskItem | null;
  pendingApprovals: ApprovalItem[];
}) {
  return (
    <div style={{ display: "grid", gap: 16 }}>
      {screen === "home" ? <HomeRail state={state} activeTask={activeTask} pendingApprovals={pendingApprovals} /> : null}
      {screen === "agent" ? <AgentRail state={state} activeTask={activeTask} pendingApprovals={pendingApprovals} /> : null}
      {screen === "wallet" ? <WalletRail state={state} pendingApprovals={pendingApprovals} /> : null}
      {screen === "apps" ? <AppsRail state={state} /> : null}
    </div>
  );
}
