import { colors, radii } from "../styles/tokens";

export function StatusBadge({ label, tone = "default" }: { label: string; tone?: "default" | "accent" | "success" }) {
  const bg = tone === "accent" ? colors.accentSoft : tone === "success" ? "rgba(61,220,151,0.16)" : "rgba(255,255,255,0.06)";
  const fg = tone === "accent" ? "#d8d2ff" : tone === "success" ? colors.success : colors.soft;

  return (
    <span
      style={{
        display: "inline-flex",
        alignItems: "center",
        padding: "6px 10px",
        borderRadius: radii.sm,
        background: bg,
        color: fg,
        fontSize: 12,
        border: `1px solid ${colors.panelBorder}`,
      }}
    >
      {label}
    </span>
  );
}
