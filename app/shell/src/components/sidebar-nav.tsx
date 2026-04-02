import type { ScreenKey } from "../types/system";

const items: { key: ScreenKey; label: string }[] = [
  { key: "home", label: "Home" },
  { key: "agent", label: "Agent" },
  { key: "wallet", label: "Wallet" },
  { key: "apps", label: "Apps" },
];

export function SidebarNav({ current, onChange }: { current: ScreenKey; onChange: (screen: ScreenKey) => void }) {
  return (
    <div style={{ display: "grid", gap: 18 }}>
      <div>
        <div style={{ fontSize: 12, letterSpacing: 4, color: "#9aa3b2", marginBottom: 8 }}>SOLOS</div>
        <div style={{ fontSize: 24, fontWeight: 700 }}>Operating Layer</div>
      </div>

      <nav style={{ display: "grid", gap: 8 }}>
        {items.map((item) => (
          <button
            key={item.key}
            onClick={() => onChange(item.key)}
            style={{
              textAlign: "left",
              padding: "12px 14px",
              borderRadius: 14,
              border: "1px solid rgba(255,255,255,0.08)",
              background: current === item.key ? "rgba(110,86,255,0.18)" : "rgba(255,255,255,0.03)",
              color: "#f5f7ff",
              cursor: "pointer",
            }}
          >
            {item.label}
          </button>
        ))}
      </nav>
    </div>
  );
}
