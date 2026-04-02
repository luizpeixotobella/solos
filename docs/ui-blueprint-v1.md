# SolOS UI Blueprint v1

## Purpose

Define the first complete interaction blueprint for the SolOS MVP.

This document bridges product vision and implementation.

## Experience principle

SolOS should feel:
- operational
- intelligent
- calm
- modular
- alive

The interface should not feel like:
- a generic dashboard
- a crypto control panel
- a chat app with a skin
- a fake operating system mockup without internal logic

## Main application map

### Core screens
1. Home
2. Agent
3. Wallet
4. Apps

### Persistent system surfaces
- sidebar / dock navigation
- top status bar
- notifications and approvals presence
- ambient agent presence

## Global layout

### Sidebar / Dock
Contains:
- SolOS mark
- Home
- Agent
- Wallet
- Apps
- future Settings
- agent presence indicator

### Top bar
Contains:
- profile/environment label
- connection/system state
- wallet status summary
- notifications badge
- approvals badge

### Main workspace
Shows the active screen content.

### Context rail (optional in MVP)
Can show:
- recent actions
- pending items
- suggestions
- details for selected objects

## Screen blueprint

## 1. Home
Purpose:
- orient the user immediately
- surface what matters now

Sections:
- Welcome / Identity
- System Overview
- Quick Actions
- Wallet Snapshot
- Recent Activity
- Agent Pulse

Suggested quick actions:
- Open Workspace
- Ask Ghost
- View Wallet
- Launch Apps
- Resume Session

## 2. Agent
Purpose:
- make the agent feel like a native operational layer

Sections:
- Agent header/status
- Conversation stream
- Suggested actions
- Active tasks
- Approval panel

Core states:
- idle
- listening
- thinking
- proposing
- awaiting approval
- executing
- completed
- failed

## 3. Wallet
Purpose:
- make identity and assets visible, explicit, and controlled

Sections:
- Account Header
- Balance Summary
- Assets List
- Recent Activity
- Signature Requests

Core states:
- disconnected
- connecting
- connected
- signature pending
- failed

## 4. Apps
Purpose:
- show SolOS as an environment for modules, not just a dashboard

Sections:
- Search + Filters
- App Grid/List
- App Details Drawer
- Pinned / Recommended Apps

App metadata shown:
- name
- type
- icon
- short description
- capability badges
- open/install state

## Required reusable components

- AppShell
- SidebarNav
- TopStatusBar
- PanelCard
- StatusBadge
- QuickActionButton
- AgentPulseCard
- ConversationPanel
- TaskTimeline
- ApprovalCard
- WalletSummaryCard
- AssetList
- AppTile
- AppDetailsDrawer

## Motion and tone

Motion should be:
- subtle
- deliberate
- premium
- not flashy

Use:
- soft fades
- short slides
- restrained glow for important states
- responsive hover and press feedback

## Visual direction

Recommended mood:
- dark graphite base
- deep blue surfaces
- violet and cyan accents
- clean typography
- glass/panel treatment used lightly

Avoid:
- noisy cyberpunk excess
- cheap neon overload
- overly gamified patterns

## First demo flow

1. Launch SolOS
2. Land on Home
3. See live environment status
4. Notice Ghost is active
5. Open Agent
6. Ask: show my wallet and open my workspace
7. See the task/approval/result flow
8. Open Wallet
9. Open Apps
10. Launch a key app
11. Return to Home with updated recent activity

## MVP rule

Every screen should answer a simple question:
- Home: what matters now?
- Agent: what can the system do with me and for me?
- Wallet: what do I own, approve, and control?
- Apps: what modules make up my environment?
