import { DemoFlowPanel } from "../../components/demo-flow-panel";
import { PanelCard } from "../../components/panel-card";
import { QuickActionButton } from "../../components/quick-action-button";
import { StatusBadge } from "../../components/status-badge";
import { colors } from "../../styles/tokens";
import type { ScreenKey, SolOSState } from "../../types/system";

const quickActions: { label: string; target: ScreenKey }[] = [
  { label: "Open Workspace", target: "apps" },
  { label: "Ask Ghost", target: "agent" },
  { label: "View Wallet", target: "wallet" },
  { label: "Launch Apps", target: "apps" },
  { label: "Resume Session", target: "agent" },
];

export function HomeScreen({
  state,
  onNavigate,
  onStartDemo,
  onFocusWallet,
  onResetDemo,
}: {
  state: SolOSState;
  onNavigate: (screen: ScreenKey) => void;
  onStartDemo: () => void;
  onFocusWallet: () => void;
  onResetDemo: () => void;
}) {
  const activeTask = state.tasks.find((task) => task.status === "active" || task.status === "awaiting-approval");
  const currentStep = state.demoFlow.find((step) => step.status === "current");

  return (
    <div style={{ display: "grid", gap: 16 }}>
      <PanelCard title="Welcome" aside={<StatusBadge label="Environment Active" tone="success" />}>
        <div style={{ display: "grid", gap: 18, gridTemplateColumns: "1.3fr 0.9fr" }}>
          <div>
            <h1 style={{ margin: 0, fontSize: 36 }}>Good evening, {state.session.displayName}</h1>
            <p style={{ color: colors.soft, maxWidth: 760 }}>
              SolOS is active. Your environment, Ghost, wallet, and apps are aligned inside one operational layer.
            </p>
            <div style={{ display: "flex", gap: 10, flexWrap: "wrap" }}>
              <QuickActionButton label="Start guided demo" onClick={onStartDemo} active />
              <QuickActionButton label="Surface wallet" onClick={onFocusWallet} />
              <QuickActionButton label="Reset shell state" onClick={onResetDemo} />
            </div>
          </div>

          <div
            style={{
              padding: 16,
              borderRadius: 18,
              background: "linear-gradient(180deg, rgba(110,86,255,0.18), rgba(255,255,255,0.04))",
              border: `1px solid ${colors.panelBorder}`,
              display: "grid",
              gap: 10,
            }}
          >
            <div style={{ fontSize: 12, letterSpacing: 2, color: colors.muted }}>LIVE DEMO STATE</div>
            <strong style={{ fontSize: 22 }}>{currentStep?.title ?? "Ready"}</strong>
            <div style={{ color: colors.soft }}>{currentStep?.summary ?? "The shell is ready to replay the end-to-end flow."}</div>
            <div style={{ display: "grid", gap: 6, marginTop: 6, color: colors.soft }}>
              <div>Ghost: {state.agent.status}</div>
              <div>Pending approvals: {state.agent.pendingApprovals}</div>
              <div>Runtime modules active: {state.apps.filter((app) => app.status === "active").length}</div>
            </div>
          </div>
        </div>
      </PanelCard>

      <div style={{ display: "grid", gridTemplateColumns: "repeat(3, 1fr)", gap: 16 }}>
        <PanelCard title="System Overview" aside={<StatusBadge label={state.system.syncState} tone="accent" />}>
          <div style={{ display: "grid", gap: 8, color: colors.soft }}>
            <div>Version: {state.system.version}</div>
            <div>Notifications: {state.system.notificationsCount}</div>
            <div>Approvals: {state.system.approvalsCount}</div>
          </div>
        </PanelCard>

        <PanelCard title="Wallet Snapshot" aside={<StatusBadge label={state.wallet.network} />}>
          <div style={{ display: "grid", gap: 8, color: colors.soft }}>
            <div>Address: {state.wallet.address}</div>
            <div>
              Primary balance: {state.wallet.balances[0]?.value} {state.wallet.balances[0]?.label}
            </div>
            <div>Assets tracked: {state.wallet.assets.length}</div>
          </div>
        </PanelCard>

        <PanelCard title="Agent Pulse" aside={<StatusBadge label={state.agent.status} />}>
          <div style={{ display: "grid", gap: 8, color: colors.soft }}>
            <div>Pending approvals: {state.agent.pendingApprovals}</div>
            <div>Last action: {state.agent.recentActions[0]}</div>
            <div>Current task: {activeTask?.title ?? "No active task"}</div>
          </div>
        </PanelCard>
      </div>

      <div style={{ display: "grid", gridTemplateColumns: "1.4fr 1fr", gap: 16 }}>
        <PanelCard title="Quick Actions">
          <div style={{ display: "flex", gap: 10, flexWrap: "wrap" }}>
            {quickActions.map((action) => (
              <QuickActionButton key={action.label} label={action.label} onClick={() => onNavigate(action.target)} />
            ))}
          </div>
        </PanelCard>

        <PanelCard title="Recent Apps">
          <ul style={{ margin: 0, paddingLeft: 18, color: colors.soft, display: "grid", gap: 8 }}>
            {state.apps.map((app) => (
              <li key={app.id}>
                {app.name} · {app.kind} · {app.status}
              </li>
            ))}
          </ul>
        </PanelCard>
      </div>

      <div style={{ display: "grid", gridTemplateColumns: "1.2fr 1fr", gap: 16 }}>
        <PanelCard title="Recent Activity">
          <ul style={{ margin: 0, paddingLeft: 18, color: colors.soft, display: "grid", gap: 8 }}>
            {state.agent.recentActions.map((item) => (
              <li key={item}>{item}</li>
            ))}
          </ul>
        </PanelCard>

        <PanelCard title="System Intent">
          <p style={{ color: colors.soft, marginTop: 0 }}>
            SolOS is being shaped as a calm, intelligent operating layer for agents, apps, identity, and digital ownership.
          </p>
          <div style={{ display: "grid", gap: 10 }}>
            <StatusBadge label="Structure Phase" tone="accent" />
            {activeTask ? <StatusBadge label={activeTask.status.replace("-", " ")} /> : null}
          </div>
        </PanelCard>
      </div>

      <DemoFlowPanel steps={state.demoFlow} />
    </div>
  );
}
