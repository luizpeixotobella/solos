import { useMemo, useState } from "react";
import { PanelCard } from "../../components/panel-card";
import { QuickActionButton } from "../../components/quick-action-button";
import { StatusBadge } from "../../components/status-badge";
import { colors } from "../../styles/tokens";
import type { AppDefinition } from "../../types/system";

const filters = ["All", "Local", "Web", "dApp", "Hybrid", "Pinned"] as const;
type FilterKey = (typeof filters)[number];

export function AppsScreen({ apps, onLaunchApp }: { apps: AppDefinition[]; onLaunchApp: (appId: AppDefinition["id"]) => void }) {
  const [filter, setFilter] = useState<FilterKey>("All");

  const visibleApps = useMemo(() => {
    if (filter === "All") return apps;
    if (filter === "Pinned") return apps.filter((app) => app.status === "pinned" || app.status === "active");
    if (filter === "dApp") return apps.filter((app) => app.kind === "dapp");
    return apps.filter((app) => app.kind === filter.toLowerCase());
  }, [apps, filter]);

  const activeApps = apps.filter((app) => app.status === "active");

  return (
    <div style={{ display: "grid", gap: 16 }}>
      <PanelCard title="Apps" aside={<StatusBadge label={`${apps.length} modules`} />}>
        <h1 style={{ margin: 0, fontSize: 34 }}>App runtime surface</h1>
        <p style={{ color: colors.soft, maxWidth: 760 }}>
          SolOS treats apps as modules of one environment: local, web, dApp, or hybrid — all visible through a coherent system layer.
        </p>
      </PanelCard>

      <div style={{ display: "grid", gridTemplateColumns: "1.3fr 0.7fr", gap: 16 }}>
        <PanelCard title="Filters">
          <div style={{ display: "flex", gap: 10, flexWrap: "wrap" }}>
            {filters.map((item) => (
              <QuickActionButton key={item} label={item} onClick={() => setFilter(item)} active={filter === item} />
            ))}
          </div>
        </PanelCard>

        <PanelCard title="Runtime Summary" aside={<StatusBadge label={`${activeApps.length} active`} tone="accent" />}>
          <div style={{ display: "grid", gap: 8, color: colors.soft }}>
            <div>Visible now: {visibleApps.length}</div>
            <div>Active modules: {activeApps.map((app) => app.name).join(", ") || "None"}</div>
            <div>Pinned surfaces: {apps.filter((app) => app.status === "pinned" || app.status === "active").length}</div>
          </div>
        </PanelCard>
      </div>

      <div style={{ display: "grid", gridTemplateColumns: "repeat(3, 1fr)", gap: 16 }}>
        {visibleApps.map((app) => (
          <PanelCard key={app.id} title={app.name} aside={<StatusBadge label={app.kind} tone="accent" />}>
            <p style={{ color: colors.soft }}>{app.description}</p>
            <p style={{ color: colors.muted }}>Status: {app.status}</p>
            <p style={{ color: colors.muted }}>Capabilities: {app.capabilities.join(", ")}</p>
            <div style={{ marginTop: 12, display: "flex", gap: 10, flexWrap: "wrap" }}>
              <QuickActionButton
                label={app.status === "active" ? "Running" : "Open"}
                active={app.status === "active"}
                onClick={() => onLaunchApp(app.id)}
              />
            </div>
          </PanelCard>
        ))}
      </div>
    </div>
  );
}
