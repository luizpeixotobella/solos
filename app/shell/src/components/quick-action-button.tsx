import type { MouseEventHandler } from "react";
import { colors, radii } from "../styles/tokens";

export function QuickActionButton({
  label,
  onClick,
  active = false,
}: {
  label: string;
  onClick?: MouseEventHandler<HTMLButtonElement>;
  active?: boolean;
}) {
  return (
    <button
      onClick={onClick}
      style={{
        padding: "10px 14px",
        borderRadius: radii.sm,
        background: active ? colors.accent : colors.accentSoft,
        color: colors.text,
        border: `1px solid ${colors.panelBorder}`,
        cursor: "pointer",
        fontWeight: 500,
      }}
    >
      {label}
    </button>
  );
}
