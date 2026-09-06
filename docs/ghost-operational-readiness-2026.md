# Ghost Operational Readiness 2026

## What changed

Ghost is now treated less like a chat surface and more like an operating agent that has to prove readiness before it acts inside SolOS.

The new runtime snapshot adds `ghost.operationalReadiness`, a structured assessment that the native shell can display directly. It measures Ghost across six pillars:

- grounded research and RAG
- long-term memory
- tool and MCP boundary
- human approval lane
- observability and evals
- language and tone mediation

This keeps the product honest. Ghost may be present in the shell before it is fully trusted to act.

## Research basis

The 2026 direction of AI agents is converging around the same operating constraints:

- OpenAI's Agents documentation centers tool use, MCP integrations, guardrails, human review, state, and tracing as first-class parts of agent workflows.
- OpenAI's Responses API exposes web search and file search as tools, making grounding and private knowledge retrieval part of the agent substrate rather than separate side systems.
- OpenAI's safety guidance explicitly recommends tool approvals, guardrails, and review points for agent workflows that touch external systems.
- Anthropic's Model Context Protocol defines a standard way to expose tools, resources, and prompts to AI applications, which makes connector boundaries more important.
- Google's ADK memory documentation separates session state from longer-term memory services, reinforcing that agents need continuity beyond a single conversation.

References:

- https://developers.openai.com/api/docs/guides/agents
- https://developers.openai.com/api/docs/guides/tools-web-search
- https://developers.openai.com/api/docs/guides/tools-file-search
- https://developers.openai.com/api/docs/guides/agent-builder-safety
- https://www.anthropic.com/news/model-context-protocol
- https://adk.dev/sessions/memory/

## Ghost readiness model

### Grounded research and RAG

Ghost should distinguish local runtime facts from external claims. If Brave or another user-owned retrieval provider is not configured, Ghost should say that grounding is unavailable instead of pretending to have fresh context.

### Long-term memory

Ghost's repo-local knowledge cache is the first memory substrate. The next step is to split memory into classes:

- session facts
- durable user preferences
- project documentation
- revocable sensitive context

This matters because memory without scope becomes surveillance. Memory with scope becomes continuity.

### Tool and MCP boundary

The rise of MCP-style connectors makes tool access easier and riskier. Ghost should never treat a discovered tool as automatically safe. SolOS should maintain a capability manifest with:

- read/write/sensitive scope
- required approval level
- data exposure class
- audit output
- revocation path

### Human approval lane

The Ghost approval lane must stay explicit for account-linked, billable, wallet, filesystem write, shell, network, and public-posting actions.

This aligns the Ghost runtime with SolOS's operating thesis: the system can be agentic without hiding cost, identity, signing, or irreversible action.

### Observability and evals

Ghost needs traces before autonomy. A useful trace should preserve:

- user intent
- retrieved context
- tool candidates
- selected tool calls
- approval decision
- action result
- user-visible outcome

The current runtime snapshot exposes state and activity, but does not yet persist traces or grade task outcomes.

The next slice adds a bounded autonomy governor. It projects minimized ledger metadata into `ghost.autonomyGovernor` and makes the progression explicit: `observe` → `propose-with-evidence` → `approval-bound`. This is the SolOS **Just Intelligent (JI)** layer: better context-sensitive routing, not silent permission escalation. A verified outcome can improve the proposal lane; it cannot authorize unattended host writes, network calls, wallet actions, or public posts.

### Language and tone mediation

Language support is part of operating intelligence. Ghost should route intent, approval explanations, retrieved evidence, and final responses through the user's active language and register.

## Implementation status

Implemented now:

- runtime-core emits `ghost.operationalReadiness`
- native runtime bridge parses readiness status, summary, and pillar lines
- `GhostRuntime` exposes readiness fields to QML
- Agent screen shows an Operational Readiness card

Still missing:

- persistent trace log
- autonomy governor connected to reviewed outcomes and a signed policy
- typed tool manifest
- real task/action router
- memory classes and revocation UI
- provider-neutral retrieval adapters beyond Brave

## Product rule

Ghost should be allowed to speak before it acts.

Ghost should be allowed to research before it executes.

Ghost should be allowed to suggest before it writes, signs, pays, posts, or changes host state.
