import { PanelCard } from "../../components/panel-card";
import { StatusBadge } from "../../components/status-badge";
import { colors } from "../../styles/tokens";
import type { ApprovalItem, SolOSState } from "../../types/system";

export function WalletScreen({ state, pendingApprovals }: { state: SolOSState; pendingApprovals: ApprovalItem[] }) {
  return (
    <div style={{ display: "grid", gap: 16 }}>
      <PanelCard title="Wallet" aside={<StatusBadge label={state.wallet.status} />}>
        <h1 style={{ margin: 0, fontSize: 34 }}>Wallet not configured</h1>
        <p style={{ color: colors.soft, maxWidth: 760 }}>
          No account, balance or assets are connected. Use the native Wallet to configure a public Polygon address for read-only queries; saving an address is not proof of control.
        </p>
      </PanelCard>

      <div style={{ display: "grid", gridTemplateColumns: "repeat(3, 1fr)", gap: 16 }}>
        <PanelCard title="Account Header">
          <div style={{ display: "grid", gap: 8, color: colors.soft }}>
            <div>Network: {state.wallet.network}</div>
            <div>Address: {state.wallet.address}</div>
            <div>Status: {state.wallet.status}</div>
          </div>
        </PanelCard>

        <PanelCard title="Balance Summary">
          <div style={{ display: "grid", gap: 8, color: colors.soft }}>
            {state.wallet.balances.length === 0 && <div>Unavailable — no live query</div>}
            {state.wallet.balances.map((balance) => (
              <div key={balance.label}>
                {balance.label}: {balance.value}
              </div>
            ))}
          </div>
        </PanelCard>

        <PanelCard title="Signature Requests" aside={<StatusBadge label={`${pendingApprovals.length} pending`} tone="accent" />}>
          <p style={{ color: colors.soft, marginTop: 0 }}>
            {pendingApprovals.length > 0
              ? "An approval is waiting for confirmation before workspace actions continue."
              : "No signature or approval requests are waiting right now."}
          </p>
        </PanelCard>
      </div>

      <PanelCard title="Assets List">
        <div style={{ display: "grid", gap: 10 }}>
          {state.wallet.assets.length === 0 && <div>Not queried</div>}
          {state.wallet.assets.map((asset) => (
            <div
              key={asset.id}
              style={{
                display: "flex",
                justifyContent: "space-between",
                gap: 12,
                padding: 14,
                borderRadius: 14,
                background: "rgba(255,255,255,0.04)",
                color: colors.soft,
              }}
            >
              <span>{asset.name}</span>
              <span>
                {asset.amount} {asset.symbol}
              </span>
            </div>
          ))}
        </div>
      </PanelCard>
    </div>
  );
}
