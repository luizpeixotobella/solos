import { QuickActionButton } from "./quick-action-button";
import { StatusBadge } from "./status-badge";
import { colors } from "../styles/tokens";
import type { ApprovalItem } from "../types/system";

export function ApprovalItemCard({
  approval,
  onApprove,
  onDeny,
}: {
  approval: ApprovalItem;
  onApprove: (approvalId: string) => void;
  onDeny: (approvalId: string) => void;
}) {
  return (
    <div style={{ padding: 14, borderRadius: 14, background: "rgba(255,255,255,0.04)" }}>
      <div style={{ display: "flex", justifyContent: "space-between", gap: 12, marginBottom: 8 }}>
        <strong>{approval.title}</strong>
        <StatusBadge label={approval.status} tone="accent" />
      </div>
      <p style={{ color: colors.soft, marginTop: 0 }}>{approval.description}</p>
      <p style={{ color: colors.muted }}>{approval.impact}</p>
      <div style={{ display: "flex", gap: 10 }}>
        <QuickActionButton label="Approve" onClick={() => onApprove(approval.id)} />
        <QuickActionButton label="Deny" onClick={() => onDeny(approval.id)} />
      </div>
    </div>
  );
}
