import { ApprovalItemCard } from "../../components/approval-item-card";
import { ConversationMessage } from "../../components/conversation-message";
import { PanelCard } from "../../components/panel-card";
import { QuickActionButton } from "../../components/quick-action-button";
import { StatusBadge } from "../../components/status-badge";
import { TaskItemCard } from "../../components/task-item-card";
import { colors } from "../../styles/tokens";
import type { ApprovalItem, ScreenKey, SolOSState, TaskItem } from "../../types/system";

const suggestedActions: { label: string; target: ScreenKey; mode?: "wallet" }[] = [
  { label: "Show wallet summary", target: "wallet", mode: "wallet" },
  { label: "Open workspace", target: "apps" },
  { label: "Review approvals", target: "wallet" },
  { label: "Launch recent apps", target: "apps" },
];

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
