use serde::Serialize;

#[derive(Serialize)]
pub(crate) struct QuickAction {
    title: String,
    subtitle: String,
    description: String,
}

#[derive(Serialize)]
#[allow(non_snake_case)]
pub(crate) struct ApprovalEntry {
    id: String,
    title: String,
    description: String,
    requestedBy: String,
    capability: String,
    scope: String,
    risk: String,
    status: String,
    createdAt: String,
}

#[derive(Serialize)]
pub(crate) struct AppEntry {
    id: String,
    name: String,
    subtitle: String,
    description: String,
    status: String,
    capability: String,
    launch_target: String,
}

pub(crate) fn build_quick_actions() -> Vec<QuickAction> {
    vec![
        QuickAction {
            title: "Inspect Ghost data layer".into(),
            subtitle: "Input signals".into(),
            description: "Read host runtime facts, config, network state, and future user context as structured inputs for Ghost.".into(),
        },
        QuickAction {
            title: "Inspect Ghost results layer".into(),
            subtitle: "Synthesis".into(),
            description: "Combine local runtime evidence with Brave research results and condense them into useful summaries for the shell.".into(),
        },
        QuickAction {
            title: "Promote Ghost algorithms into actions".into(),
            subtitle: "Next module".into(),
            description: "Turn ranked Ghost conclusions into explicit SolOS tasks, approvals, and app-level actions instead of keeping them purely descriptive.".into(),
        },
    ]
}

pub(crate) fn build_approvals() -> Vec<ApprovalEntry> {
    vec![
        ApprovalEntry {
            id: "approval-ghost-web-access".into(),
            title: "Bind Ghost web research to the user's own Brave key".into(),
            description: "Ghost should open Brave's API key page for the SolOS user, let them pay or subscribe on their own account, then return and configure a repo-local key instead of sharing the developer key.".into(),
            requestedBy: "ghost-brain".into(),
            capability: "web.search.read".into(),
            scope: "ghost onboarding -> Brave key acquisition -> ignored solos/config/ghost.local.json".into(),
            risk: "medium".into(),
            status: "pending".into(),
            createdAt: "ghost-module-bootstrap".into(),
        },
        ApprovalEntry {
            id: "approval-ghost-task-execution".into(),
            title: "Connect Ghost conclusions to executable tasks".into(),
            description: "Prepare the next module so Ghost can transform ranked conclusions into app launches, commands, and approval requests inside SolOS.".into(),
            requestedBy: "ghost-brain".into(),
            capability: "task.intent.dispatch".into(),
            scope: "Ghost pipeline -> SolOS task/action router".into(),
            risk: "high".into(),
            status: "pending".into(),
            createdAt: "ghost-module-bootstrap".into(),
        },
    ]
}

pub(crate) fn build_app_registry() -> Vec<AppEntry> {
    vec![
        AppEntry {
            id: "workspace".into(),
            name: "Workspace".into(),
            subtitle: "Operating layer context".into(),
            description: "Coordinates user tasks, notes, and live environment state above the runtime intermediary.".into(),
            status: "available".into(),
            capability: "app.open.safe".into(),
            launch_target: "screen:Home".into(),
        },
        AppEntry {
            id: "approval-lane".into(),
            name: "Approval Lane".into(),
            subtitle: "Policy boundary".into(),
            description: "Surfaces runtime-mediated host actions that require explicit consent before execution.".into(),
            status: "available".into(),
            capability: "app.open.safe".into(),
            launch_target: "screen:Agent".into(),
        },
        AppEntry {
            id: "wallet-hub".into(),
            name: "Wallet Hub".into(),
            subtitle: "Ownership surface".into(),
            description: "Keeps identity, balances, and signing visible through explicit runtime-mediated flows.".into(),
            status: "available".into(),
            capability: "app.open.safe".into(),
            launch_target: "screen:Wallet".into(),
        },
        AppEntry {
            id: "ghost-console".into(),
            name: "Ghost Console".into(),
            subtitle: "Layered intelligence module".into(),
            description: "Turns runtime data plus optional Brave research into structured results and next-step algorithms inside SolOS.".into(),
            status: "available".into(),
            capability: "app.open.safe".into(),
            launch_target: "screen:Agent".into(),
        },
        AppEntry {
            id: "solos-pulso".into(),
            name: "SolOS Pulso".into(),
            subtitle: "Controlled Alpha adapter".into(),
            description: "Opens the consented Pulso Alpha through an exact allowlisted route while its native SolOS surface remains a later product slice.".into(),
            status: "connected-alpha".into(),
            capability: "app.open.safe".into(),
            launch_target: "https://luiz-bella-artes.net/solos/pulso".into(),
        },
    ]
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn catalog_keeps_operating_primitives_visible() {
        assert_eq!(build_quick_actions().len(), 3);
        assert_eq!(build_approvals().len(), 2);
        assert_eq!(build_app_registry().len(), 5);
        assert!(build_app_registry()
            .iter()
            .all(|app| app.capability == "app.open.safe"));
    }
}
