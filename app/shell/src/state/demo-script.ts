import type { DemoStep, ScreenKey } from "../types/system";

function step(id: string, title: string, summary: string, target: ScreenKey): DemoStep {
  return {
    id,
    title,
    summary,
    target,
    status: id === "demo-1" ? "current" : "upcoming",
  };
}

export const demoScript: DemoStep[] = [
  step("demo-1", "Land on Home", "The shell immediately shows environment status, wallet presence, and Ghost activity.", "home"),
  step("demo-2", "Open Agent", "Ghost receives a system-level request instead of acting like a detached chat widget.", "agent"),
  step("demo-3", "Review approval", "Sensitive or consequential actions stay visible, bounded, and explicit.", "agent"),
  step("demo-4", "Inspect Wallet", "Identity, balances, and signature awareness remain first-class system surfaces.", "wallet"),
  step("demo-5", "Launch Workspace", "Apps behave like modules of one environment, not separate disconnected products.", "apps"),
];
