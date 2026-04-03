import { PanelCard } from "./panel-card";
import { StatusBadge } from "./status-badge";
import { colors } from "../styles/tokens";
import type { DemoStep } from "../types/system";

export function DemoFlowPanel({ steps, compact = false }: { steps: DemoStep[]; compact?: boolean }) {
  return (
    <PanelCard title="Demo Flow" aside={<StatusBadge label={`${steps.length} beats`} tone="accent" />}>
      <div style={{ display: "grid", gap: compact ? 10 : 12 }}>
        {steps.map((step, index) => (
          <div key={step.id} style={{ padding: compact ? 10 : 12, borderRadius: 12, background: step.status === "current" ? "rgba(110,86,255,0.14)" : "rgba(255,255,255,0.04)" }}>
            <div style={{ display: "flex", justifyContent: "space-between", gap: 10, marginBottom: 6, alignItems: "center" }}>
              <strong>{step.title}</strong>
              <div style={{ display: "flex", gap: 8, alignItems: "center" }}>
                <span style={{ color: colors.muted }}>0{index + 1}</span>
                <StatusBadge label={step.status} tone={step.status === "completed" ? "success" : step.status === "current" ? "accent" : "default"} />
              </div>
            </div>
            <div style={{ color: colors.soft }}>{step.summary}</div>
          </div>
        ))}
      </div>
    </PanelCard>
  );
}
