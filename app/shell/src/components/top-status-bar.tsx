import { QuickActionButton } from "./quick-action-button";
import type { AgentState, SystemStatus, UserSession, WalletState } from "../types/system";

export function TopStatusBar({
  session,
  system,
  agent,
  wallet,
  onReset,
}: {
  session: UserSession;
  system: SystemStatus;
  agent: AgentState;
  wallet: WalletState;
  onReset?: () => void;
}) {
  return (
    <div style={{ display: "flex", justifyContent: "space-between", alignItems: "center", gap: 16 }}>
      <div>
        <div style={{ fontSize: 12, color: "#9aa3b2" }}>{session.environmentMode}</div>
        <div style={{ fontSize: 18, fontWeight: 600 }}>Ghost active · {session.displayName}</div>
      </div>
      <div style={{ display: "flex", gap: 12, flexWrap: "wrap", alignItems: "center", color: "#c8d0df", fontSize: 14 }}>
        <span>System: {system.syncState}</span>
        <span>Agent: {agent.status}</span>
        <span>Wallet: {wallet.status}</span>
        <span>Alerts: {system.notificationsCount}</span>
        <span>Approvals: {system.approvalsCount}</span>
        {onReset ? <QuickActionButton label="Reset demo" onClick={onReset} /> : null}
      </div>
    </div>
  );
}
