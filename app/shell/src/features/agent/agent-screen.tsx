import { ApprovalItemCard } from "../../components/approval-item-card";
import { ConversationMessage } from "../../components/conversation-message";
import { PanelCard } from "../../components/panel-card";
import { QuickActionButton } from "../../components/quick-action-button";
import { StatusBadge } from "../../components/status-badge";
import { TaskItemCard } from "../../components/task-item-card";
import { colors } from "../../styles/tokens";
import type { ApprovalItem, GhostRequestClass, ScreenKey, SolOSState, TaskItem } from "../../types/system";

const suggestedActions: { label: string; target: ScreenKey; mode?: "wallet" }[] = [
  { label: "Show wallet summary", target: "wallet", mode: "wallet" },
  { label: "Open workspace", target: "apps" },
  { label: "Review approvals", target: "wallet" },
  { label: "Launch recent apps", target: "apps" },
];

function TraceField({ label, value }: { label: string; value: string }) {
  return (
    <div style={{ display: "grid", gap: 4 }}>
      <span style={{ color: colors.muted, fontSize: 12, textTransform: "uppercase", letterSpacing: 1 }}>{label}</span>
      <span style={{ color: colors.soft, lineHeight: 1.45 }}>{value}</span>
    </div>
  );
}

function RequestClassTile({ item }: { item: GhostRequestClass }) {
  return (
    <div
      style={{
        border: `1px solid ${colors.panelBorder}`,
        borderRadius: 12,
        padding: 14,
        background: "rgba(255,255,255,0.02)",
        display: "grid",
        gap: 10,
      }}
    >
      <div style={{ display: "flex", justifyContent: "space-between", gap: 10, alignItems: "flex-start" }}>
        <strong>{item.name}</strong>
        <StatusBadge label={item.status} />
      </div>
      <TraceField label="Safety" value={item.safetyLevel} />
      <TraceField label="Tools" value={item.requiredTools} />
      <TraceField label="Approval" value={item.approvalNeeds} />
      <TraceField label="Quota" value={item.quotaCost} />
      <TraceField label="Route" value={item.route} />
    </div>
  );
}

export function AgentScreen({
  state,
  activeTask,
  pendingApprovals,
  onNavigate,
  onApprove,
  onDeny,
  onFocusWallet,
}: {
  state: SolOSState;
  activeTask: TaskItem | null;
  pendingApprovals: ApprovalItem[];
  onNavigate: (screen: ScreenKey) => void;
  onApprove: (approvalId: string) => void;
  onDeny: (approvalId: string) => void;
  onFocusWallet: () => void;
}) {
  return (
    <div style={{ display: "grid", gap: 16 }}>
      <PanelCard title="Ghost" aside={<StatusBadge label={state.agent.status} tone="accent" />}>
        <h1 style={{ margin: 0, fontSize: 34 }}>Ghost is active</h1>
        <p style={{ color: colors.soft, maxWidth: 760 }}>
          The agent layer coordinates bounded actions, surfaces approvals, and turns system intent into visible operational flows.
        </p>
      </PanelCard>

      <div style={{ display: "grid", gridTemplateColumns: "1.2fr 0.8fr", gap: 16 }}>
        <PanelCard title="Conversation Stream">
          <div style={{ display: "grid", gap: 12 }}>
            {state.conversation.map((message) => (
              <ConversationMessage key={message.id} message={message} />
            ))}
          </div>
        </PanelCard>

        <PanelCard title="Suggested Actions">
          <div style={{ display: "flex", gap: 10, flexWrap: "wrap" }}>
            {suggestedActions.map((item) => (
              <QuickActionButton
                key={item.label}
                label={item.label}
                onClick={() => {
                  if (item.mode === "wallet") {
                    onFocusWallet();
                    return;
                  }
                  onNavigate(item.target);
                }}
              />
            ))}
          </div>
        </PanelCard>
      </div>

      <PanelCard title={state.agent.requestClassifier.title} aside={<StatusBadge label="route-visible" tone="accent" />}>
        <div style={{ display: "grid", gap: 14 }}>
          <p style={{ color: colors.soft, margin: 0 }}>{state.agent.requestClassifier.summary}</p>
          <p style={{ color: colors.muted, margin: 0 }}>Example: {state.agent.requestClassifier.exampleRequest}</p>
          <div style={{ display: "grid", gridTemplateColumns: "repeat(auto-fit, minmax(260px, 1fr))", gap: 12 }}>
            {state.agent.requestClassifier.classes.map((item) => (
              <RequestClassTile key={item.name} item={item} />
            ))}
          </div>
        </div>
      </PanelCard>

      <div style={{ display: "grid", gridTemplateColumns: "repeat(auto-fit, minmax(280px, 1fr))", gap: 16 }}>
        <PanelCard title="Action Trace" aside={<StatusBadge label={state.agent.actionTrace.outcome} />}>
          <div style={{ display: "grid", gap: 12 }}>
            <TraceField label="Trace" value={state.agent.actionTrace.traceId} />
            <TraceField label="Request" value={state.agent.actionTrace.request} />
            <TraceField label="Data" value={state.agent.actionTrace.data} />
            <TraceField label="Result Target" value={state.agent.actionTrace.resultTarget} />
            <TraceField label="Algorithm Route" value={state.agent.actionTrace.algorithmRoute} />
            <TraceField label="Quota" value={state.agent.actionTrace.quotaCost} />
            <TraceField label="Approval" value={state.agent.actionTrace.approvalRequired} />
          </div>
        </PanelCard>

        <PanelCard title="Route Explanation" aside={<StatusBadge label={state.agent.routeExplanation.safetyLevel} />}>
          <div style={{ display: "grid", gap: 12 }}>
            <TraceField label="Selected Class" value={state.agent.routeExplanation.selectedClass} />
            <TraceField label="Selected Route" value={state.agent.routeExplanation.selectedRoute} />
            <TraceField label="Why" value={state.agent.routeExplanation.explanation} />
            <TraceField label="Approval Policy" value={state.agent.routeExplanation.approvalPolicy} />
            <TraceField label="Quota Policy" value={state.agent.routeExplanation.quotaPolicy} />
            <TraceField label="Next" value={state.agent.routeExplanation.nextStep} />
          </div>
        </PanelCard>
      </div>

      <div style={{ display: "grid", gridTemplateColumns: "repeat(2, 1fr)", gap: 16 }}>
        <PanelCard title="Active Tasks" aside={<StatusBadge label={activeTask ? activeTask.status : "stable"} />}>
          <div style={{ display: "grid", gap: 12 }}>
            {state.tasks.map((task) => (
              <TaskItemCard key={task.id} task={task} />
            ))}
          </div>
        </PanelCard>

        <PanelCard title="Approval Panel">
          {pendingApprovals.length === 0 ? (
            <p style={{ color: colors.soft, marginTop: 0 }}>No pending approvals. Sensitive actions are currently settled.</p>
          ) : (
            <div style={{ display: "grid", gap: 12 }}>
              {pendingApprovals.map((approval) => (
                <ApprovalItemCard key={approval.id} approval={approval} onApprove={onApprove} onDeny={onDeny} />
              ))}
            </div>
          )}
        </PanelCard>
      </div>
    </div>
  );
}
