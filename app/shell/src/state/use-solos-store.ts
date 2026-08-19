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
import type { AgentConversationMessage, ApprovalItem, AppDefinition, DemoStep, GhostResolutionLoop, SolOSState, TaskItem } from "../types/system";

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

  const selectedResolution = agent.resolutionLoop.resolutions.find(
    (resolution) => resolution.id === agent.resolutionLoop.selectedId,
  ) ?? null;

  const updateResolutionLoop = (transform: (loop: GhostResolutionLoop) => GhostResolutionLoop) => {
    setAgent((current) => ({ ...current, resolutionLoop: transform(current.resolutionLoop) }));
  };

  const selectResolution = (resolutionId: string) => {
    const target = agent.resolutionLoop.resolutions.find((resolution) => resolution.id === resolutionId);
    if (!target || target.readiness !== "ready") return;

    updateResolutionLoop((loop) => ({
      ...loop,
      selectedId: resolutionId,
      summary: "A ready objective is selected. The next transition builds the plan and opens the approval boundary.",
      resolutions: loop.resolutions.map((resolution) =>
        resolution.id === resolutionId
          ? {
              ...resolution,
              status: "selected" as const,
              progress: 20,
              currentStep: "Build a bounded plan",
              resultSummary: "Objective selected; execution has not started.",
              evidence: ["Objective selected"],
              steps: resolution.steps.map((step, index) => ({
                ...step,
                status: index === 0 ? ("completed" as const) : index === 1 ? ("active" as const) : ("pending" as const),
                result: index === 0 ? "Bounded local app action." : "",
              })),
            }
          : resolution,
      ),
    }));
  };

  const startResolution = (resolutionId: string) => {
    const target = agent.resolutionLoop.resolutions.find((resolution) => resolution.id === resolutionId);
    if (!target || target.status !== "selected") return;

    updateResolutionLoop((loop) => ({
      ...loop,
      summary: "The selected resolution is waiting for explicit user approval.",
      resolutions: loop.resolutions.map((resolution) =>
        resolution.id === resolutionId
          ? {
              ...resolution,
              status: "awaiting-approval" as const,
              progress: 50,
              currentStep: "Ask for explicit approval",
              resultSummary: "Plan prepared. No app action runs before the visible approval decision.",
              evidence: [...resolution.evidence, "Bounded plan prepared"],
              steps: resolution.steps.map((step) => {
                if (step.id === "understand") return { ...step, status: "completed" as const, result: "Objective classified as a bounded local app action." };
                if (step.id === "plan") return { ...step, status: "completed" as const, result: "Plan fixed: approval → app.open.safe → app.state.read." };
                if (step.id === "approval") return { ...step, status: "active" as const };
                return step;
              }),
            }
          : resolution,
      ),
    }));

    const approval: ApprovalItem = {
      id: "approval-workspace",
      title: "Resume workspace session",
      description: "Open the Workspace module through app.open.safe and verify its active state.",
      impact: "Bounded local app action. No wallet signing, shell command, or public send.",
      status: "pending",
    };
    setApprovals([approval]);
    syncAgentCounts([approval]);
    setTasks((current) => current.map((task) => task.id === "task-workspace"
      ? { ...task, status: "awaiting-approval" as const, detail: "Bounded plan prepared; explicit approval required." }
      : task));
    setAgent((current) => ({ ...current, status: "awaiting-approval", currentTask: target.title }));
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

    if (selectedResolution?.status === "selected") {
      startResolution(selectedResolution.id);
    }

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
      resolutionLoop: {
        ...current.resolutionLoop,
        summary: "The selected objective is resolved with approval, capability result, and state verification preserved end to end.",
        resolutions: current.resolutionLoop.resolutions.map((resolution) =>
          resolution.id === current.resolutionLoop.selectedId
            ? {
                ...resolution,
                status: "resolved" as const,
                progress: 100,
                currentStep: "Target outcome verified",
                resultSummary: "Workspace is active. Approval, mediated execution, and verification evidence are attached to this resolution.",
                evidence: [...resolution.evidence, "Approval granted", "Workspace opened", "Outcome verified"],
                steps: resolution.steps.map((step) => ({
                  ...step,
                  status: "completed" as const,
                  result:
                    step.id === "approval" ? "User approved app.open.safe for Workspace."
                    : step.id === "execute" ? "Workspace changed to active through app.open.safe."
                    : step.id === "verify" ? "app.state.read confirmed Workspace is active."
                    : step.result,
                })),
              }
            : resolution,
        ),
      },
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
      resolutionLoop: {
        ...current.resolutionLoop,
        summary: "The resolution is blocked by a recorded user denial; no capability executed.",
        resolutions: current.resolutionLoop.resolutions.map((resolution) =>
          resolution.id === current.resolutionLoop.selectedId
            ? {
                ...resolution,
                status: "blocked" as const,
                progress: 50,
                currentStep: "Stopped at approval boundary",
                resultSummary: "No action executed. The user's denial was retained as evidence.",
                evidence: [...resolution.evidence, "Approval denied; no side effect"],
                steps: resolution.steps.map((step) => step.id === "approval"
                  ? { ...step, status: "blocked" as const, result: "User denied the requested capability." }
                  : step),
              }
            : resolution,
        ),
      },
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
    selectResolution,
    startResolution,
    resetDemo,
    setApps,
    setDemoStep,
  };
}
