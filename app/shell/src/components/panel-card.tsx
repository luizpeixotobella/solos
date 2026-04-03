import type { ReactNode } from "react";
import { colors, radii } from "../styles/tokens";

export function PanelCard({ title, children, aside }: { title: string; children: ReactNode; aside?: ReactNode }) {
  return (
    <section
      style={{
        background: colors.panel,
        border: `1px solid ${colors.panelBorder}`,
        borderRadius: radii.lg,
        padding: 18,
      }}
    >
      <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center", gap: 12, marginBottom: 12 }}>
        <div style={{ fontSize: 12, color: colors.muted, letterSpacing: 2 }}>{title.toUpperCase()}</div>
        {aside}
      </div>
      {children}
    </section>
  );
}
