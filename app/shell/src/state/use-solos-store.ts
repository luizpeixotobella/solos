import { useMemo, useState } from "react";
import {
  initialAgentConversation,
  initialAgentState,
  initialApps,
  initialApprovals,
  initialSession,
  initialSystemStatus,
  initialTasks,
  initialWalletState,
} from "./mock-system";
import { demoScript } from "./demo-script";
import type { AgentConversationMessage, ApprovalItem, AppDefinition, DemoStep, SolOSState, TaskItem } from "../types/system";

function advanceDemoFlow(current: DemoStep[], stepId: string) {
  const index = current.findIndex((step) => step.id === stepId);
  if (index === -1) return current;

  return current.map((step, i) => {
    if (i < index) return { ...step, status: "completed" as const };
    if (i === index) return { ...step, status: "current" as const };
    return { ...step, status: "upcoming" as const };
  });
}

export function useSolOSStore() {
  const [session] = useState(initialSession);
  const [system, setSystem] = useState(initialSystemStatus);
  const [agent, setAgent] = useState(initialAgentState);
  const [wallet] = useState(initialWalletState);
  const [apps, setApps] = useState(initialApps);
  const [tasks, setTasks] = useState(initialTasks);
  const [approvals, setApprovals] = useState(initialApprovals);
  const [conversation, setConversation] = useState(initialAgentConversation);
  const [demoFlow, setDemoFlow] = useState(demoScript);

  const activeTask = useMemo(() => tasks.find((task) => task.status !== "done" && task.status !== "failed") ?? null, [tasks]);
  const pendingApprovals = approvals.filter((approval) => approval.status === "pending");
  const activeApp = apps.find((app) => app.status === "active") ?? null;

  const appendConversation = (message: AgentConversationMessage) => {
    setConversation((current) => [...current, message]);
  };

  const syncAgentCounts = (nextApprovals: ApprovalItem[]) => {
    const pendingCount = nextApprovals.filter((approval) => approval.status === "pending").length;

    setAgent((current) => ({ ...current, pendingApprovals: pendingCount }));
    setSystem((current) => ({ ...current, approvalsCount: pendingCount }));
  };

  const setDemoStep = (stepId: string) => {
    setDemoFlow((current) => advanceDemoFlow(current, stepId));
  };

  const resetDemo = () => {
    setSystem(initialSystemStatus);
    setAgent(initialAgentState);
    setApps(initialApps);
    setTasks(initialTasks);
    setApprovals(initialApprovals);
    setConversation(initialAgentConversation);
    setDemoFlow(demoScript);
  };

  const requestWorkspaceAccess = () => {
    const alreadyRequested = conversation.some((message) => message.id === "msg-user-workspace-request");

    setDemoStep("demo-2");
    setAgent((current) => ({
      ...current,
      status: "awaiting-approval",
      currentTask: "Resume Workspace",
      recentActions: ["Workspace request surfaced", ...current.recentActions.filter((item) => item !== "Workspace request surfaced")].slice(0, 6),
    }));

    if (!alreadyRequested) {
      setConversation((current) => [
        ...current,
        {
          id: "msg-user-workspace-request",
          role: "user",
          text: "Resume my workspace and keep the wallet summary visible.",
        },
        {
          id: "msg-agent-workspace-request",
          role: "agent",
          text: "Workspace restore is prepared. Approval stays visible before the module is opened.",
          tone: "accent",
        },
      ]);
    }
  };

  const approve = (approvalId: string) => {
    let approvedTitle = "requested action";

    const nextApprovals = approvals.map((approval) => {
      if (approval.id !== approvalId) return approval;
      approvedTitle = approval.title;
      return { ...approval, status: "approved" as const };
    });

    setApprovals(nextApprovals);
    syncAgentCounts(nextApprovals);
    setDemoStep("demo-5");

    setTasks((current) =>
      current.map((task) =>
        task.id === "task-workspace"
          ? { ...task, status: "done" as const, detail: "Workspace resumed inside the SolOS shell." }
          : task,
      ),
    );

    setApps((current) => current.map((app) => (app.id === "workspace" ? { ...app, status: "active" as const } : app)));

    setAgent((current) => ({
      ...current,
      status: "completed",
      currentTask: "Workspace resumed",
      recentActions: [`Approved: ${approvedTitle}`, ...current.recentActions].slice(0, 6),
    }));

    appendConversation({
      id: `msg-approved-${approvalId}`,
      role: "agent",
      text: `Approval received. ${approvedTitle} is now executing inside the workspace layer.`,
      tone: "accent",
    });

    setSystem((current) => ({ ...current, notificationsCount: current.notificationsCount + 1 }));
  };

  const deny = (approvalId: string) => {
    let deniedTitle = "requested action";

    const nextApprovals = approvals.map((approval) => {
      if (approval.id !== approvalId) return approval;
      deniedTitle = approval.title;
      return { ...approval, status: "denied" as const };
    });

    setApprovals(nextApprovals);
    syncAgentCounts(nextApprovals);

    setTasks((current) =>
      current.map((task) =>
        task.id === "task-workspace"
          ? { ...task, status: "failed" as const, detail: "Workspace resume was denied by the user." }
          : task,
      ),
    );

    setAgent((current) => ({
      ...current,
      status: "failed",
      currentTask: null,
      recentActions: [`Denied: ${deniedTitle}`, ...current.recentActions].slice(0, 6),
    }));

    appendConversation({
      id: `msg-denied-${approvalId}`,
      role: "agent",
      text: `${deniedTitle} was denied. No sensitive action was executed.`,
      tone: "default",
    });
  };

  const focusWallet = () => {
    setDemoStep("demo-4");
    setAgent((current) => ({
      ...current,
      status: "completed",
      currentTask: "Wallet summary surfaced",
      recentActions: ["Wallet summary surfaced", ...current.recentActions].slice(0, 6),
    }));

    appendConversation({
      id: `msg-wallet-${conversation.length + 1}`,
      role: "agent",
      text: "Wallet surface is ready. Balances, assets, and pending signatures are visible.",
      tone: "accent",
    });
  };

  const launchApp = (appId: AppDefinition["id"]) => {
    const target = apps.find((app) => app.id === appId);
    if (!target) return;

    setApps((current) =>
      current.map((app) => ({
        ...app,
        status: app.id === appId ? ("active" as const) : app.id === "workspace" ? app.status : app.status,
      })),
    );

    setAgent((current) => ({
      ...current,
      status: "executing",
      currentTask: `${target.name} active`,
      recentActions: [`Launched ${target.name}`, ...current.recentActions].slice(0, 6),
    }));

    appendConversation({
      id: `msg-app-${appId}-${conversation.length + 1}`,
      role: "system",
      text: `${target.name} is now active inside the shared environment layer.`,
      tone: "default",
    });

    setSystem((current) => ({ ...current, notificationsCount: current.notificationsCount + 1 }));
  };

  const state: SolOSState = {
    session,
    system,
    agent,
    wallet,
    apps,
    tasks,
    approvals,
    conversation,
    demoFlow,
  };

  return {
    state,
    activeTask,
    activeApp,
    pendingApprovals,
    approve,
    deny,
    focusWallet,
    launchApp,
    requestWorkspaceAccess,
    resetDemo,
    setApps,
    setDemoStep,
  };
}
