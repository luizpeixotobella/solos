import type { ReactNode } from "react";

export function AppShell({
  sidebar,
  topbar,
  rail,
  children,
}: {
  sidebar: ReactNode;
  topbar: ReactNode;
  rail?: ReactNode;
  children: ReactNode;
}) {
  return (
    <div style={{ display: "grid", gridTemplateColumns: rail ? "240px minmax(0, 1fr) 320px" : "240px 1fr", minHeight: "100vh", background: "#070a12", color: "#f5f7ff" }}>
      <aside style={{ borderRight: "1px solid rgba(255,255,255,0.08)", padding: 20 }}>{sidebar}</aside>
      <div style={{ display: "grid", gridTemplateRows: "72px 1fr", minWidth: 0 }}>
        <header style={{ borderBottom: "1px solid rgba(255,255,255,0.08)", padding: "18px 24px" }}>{topbar}</header>
        <main style={{ padding: 24, minWidth: 0 }}>{children}</main>
      </div>
      {rail ? <aside style={{ borderLeft: "1px solid rgba(255,255,255,0.08)", padding: 20 }}>{rail}</aside> : null}
    </div>
  );
}
