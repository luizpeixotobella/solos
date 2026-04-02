import { useState } from "react";
import { AppShell } from "../components/app-shell";
import { ContextRail } from "../components/context-rail";
import { SidebarNav } from "../components/sidebar-nav";
import { TopStatusBar } from "../components/top-status-bar";
import { AgentScreen } from "../features/agent/agent-screen";
import { AppsScreen } from "../features/apps/apps-screen";
import { HomeScreen } from "../features/home/home-screen";
import { WalletScreen } from "../features/wallet/wallet-screen";
import { defaultScreen } from "../state/mock-system";
import { useSolOSStore } from "../state/use-solos-store";
import type { ScreenKey } from "../types/system";

export function ShellRoot() {
  const [screen, setScreen] = useState<ScreenKey>(defaultScreen);
  const { state, activeTask, pendingApprovals, approve, deny, focusWallet, launchApp, requestWorkspaceAccess, resetDemo, setDemoStep } = useSolOSStore();

  const navigate = (next: ScreenKey) => {
    if (next === "home") setDemoStep("demo-1");
    if (next === "agent") setDemoStep(pendingApprovals.length > 0 ? "demo-3" : "demo-2");
    if (next === "wallet") setDemoStep("demo-4");
    if (next === "apps") setDemoStep("demo-5");
    setScreen(next);
  };

  const content =
    screen === "home" ? (
      <HomeScreen
        state={state}
        onNavigate={navigate}
        onStartDemo={() => {
          requestWorkspaceAccess();
          setScreen("agent");
        }}
        onFocusWallet={() => {
          focusWallet();
          setScreen("wallet");
        }}
        onResetDemo={() => {
          resetDemo();
          setScreen("home");
        }}
      />
    ) : screen === "agent" ? (
      <AgentScreen
        state={state}
        activeTask={activeTask}
        pendingApprovals={pendingApprovals}
        onNavigate={navigate}
        onApprove={(approvalId) => {
          approve(approvalId);
          setScreen("apps");
        }}
        onDeny={(approvalId) => {
          deny(approvalId);
          setScreen("home");
        }}
        onFocusWallet={() => {
          focusWallet();
          setScreen("wallet");
        }}
      />
    ) : screen === "wallet" ? (
      <WalletScreen state={state} pendingApprovals={pendingApprovals} />
    ) : (
      <AppsScreen apps={state.apps} onLaunchApp={launchApp} />
    );

  return (
    <AppShell
      sidebar={<SidebarNav current={screen} onChange={navigate} />}
      topbar={<TopStatusBar session={state.session} system={state.system} agent={state.agent} wallet={state.wallet} onReset={() => {
        resetDemo();
        setScreen("home");
      }} />}
      rail={<ContextRail screen={screen} state={state} activeTask={activeTask} pendingApprovals={pendingApprovals} />}
    >
      {content}
    </AppShell>
  );
}
