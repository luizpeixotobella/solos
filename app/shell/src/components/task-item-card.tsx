import { StatusBadge } from "./status-badge";
import { colors } from "../styles/tokens";
import type { TaskItem } from "../types/system";

export function TaskItemCard({ task }: { task: TaskItem }) {
  return (
    <div style={{ padding: 14, borderRadius: 14, background: "rgba(255,255,255,0.04)" }}>
      <div style={{ display: "flex", justifyContent: "space-between", gap: 12, marginBottom: 8 }}>
        <strong>{task.title}</strong>
        <StatusBadge label={task.status.replace("-", " ")} tone={task.status === "done" ? "success" : task.status === "awaiting-approval" ? "accent" : "default"} />
      </div>
      <div style={{ color: colors.soft }}>{task.detail}</div>
    </div>
  );
}
