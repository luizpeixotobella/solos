# Pulso Adult Alpha — Security-First Reservation

Date: 2026-08-26

Status: **P0 fail-closed implementation landed in the CMS and production database; all operational gates remain closed; not a public-launch authorization**.

## Decision

Pulso can plausibly launch as a small operational prototype if the release is defined honestly as:

> **Pulso Alpha 0 — Brazil-only, verified-adult, invite-only, text-only and human-premoderated.**

This is not approval for a global public social network, minor participation, media uploads, live streaming, direct messages, autonomous AI moderation, or self-modifying ranking.

## Current evidence

On 2026-08-26:

- the public Pulso product, protected feed and Safety Center routes returned HTTP 200 in production;
- the persistent Pulso, Founder Rewards and security-hardening migrations were present in the linked production database;
- the production Pulso tables were empty: zero topics, posts, comments, reactions, signals, safety profiles, reports and moderation items;
- publishing remained closed by default;
- the existing implementation already had authenticated posting gates, local plus provider moderation, fail-closed review, RLS, blocking, reporting, export and deletion;
- targeted lint completed without findings and the production dependency audit reported zero known vulnerabilities at that moment.

This means Pulso has a protected technical shell, not an operationally ready social network.

## Implementation checkpoint — 2026-08-26

Implemented and verified:

- production migration `202608260000_pulso_adult_alpha_zero.sql` applied successfully;
- database and environment gates must agree, with the database kill switch engaged by default;
- one-use invite token hash plus peppered recipient-email HMAC; no raw invite token, birth date or document copy is stored;
- atomic invite redemption, Brazil/policy binding and a transactional cap of ten active adults;
- social RLS denies anonymous, non-member, self-declared-only and policy-stale access;
- text-only post submission is atomic, limited to three per hour and always enters human review, even when automated moderation says `allow`;
- mutation routes require a trusted same-host Origin and use database-backed fixed-window rate limits keyed by server-side HMAC rather than raw IP;
- production `PULSO_INVITE_PEPPER` and `PULSO_RATE_LIMIT_PEPPER` secrets are stored in Render; the resulting deploy succeeded, a trusted unauthenticated mutation reached the expected `401 authentication_required`, and a foreign Origin still failed with `403 invalid_origin`;
- human moderation publishes only to the limited adult feed and records an append-style operator event;
- chronological feed is capped at twelve raw items per explicit page, with at most two visible posts per author;
- self-service export includes Alpha membership and social deletion revokes the membership;
- versioned Alpha rules are public, invite acceptance writes a purpose-bound consent receipt atomically, export includes it and social deletion revokes it;
- targeted ESLint completed without findings, the Next.js production build passed, the migration applied, and linked database lint found only two pre-existing Founder-function warnings.

Production truth immediately after the migration: `alpha_enabled=false`, registration/feed/posting gates false, `kill_switch=true`, and zero members/invites/operator events.

Runtime truth after the Render secret checkpoint: `PULSO_ALPHA_ENABLED` remains absent/false. Secret availability removes a configuration failure but does not open registration, feed or posting.

## Non-negotiable Alpha 0 boundaries

1. **Adults only:** no child or teenager accounts, including read-only social accounts.
2. **Invite and allowlist only:** begin with at most ten known adult participants.
3. **Reliable adult condition:** do not enable publishing from self-declaration alone. Store only the minimum adult-attestation result, provider/method, policy version and expiry—not a birth date or identity-document copy.
4. **Brazil-only first:** do not claim global legal compliance. Add countries only through an explicit jurisdiction gate and reviewed policy matrix.
5. **Text only:** no images, video, audio, live streaming, direct messages, location, unrestricted discovery, contact exchange or external-link previews.
6. **Human premoderation:** every Alpha 0 post waits for a human decision. When no operator is available, publishing queues instead of auto-publishing.
7. **Bounded feed:** no infinite scroll, autoplay, streaks or flood. Show at most 12 items, then require an explicit request for more.
8. **No surveillance business model:** no advertising trackers, raw-data sale, sensitive-trait inference or training on personal content by default.
9. **Reversible operations:** kill switch, fail-closed provider behavior, append-only moderation trail, appeal path, backup/restore test and incident playbook.
10. **Honest status:** call it an alpha experiment, publish its limitations and never describe it as a safe network in an absolute sense.

## Pulso da Hora: score content, never human worth

The proposed “pontuação da horinha” belongs to a post in a short time window. It must not become a permanent person score or covert social-credit profile.

Default feed:

- deterministic reverse chronology;
- maximum two posts from the same author in one page;
- explicit topic filters chosen by the user;
- fixed batch size and a visible end.

Optional `Pulso da Hora` ordering may use:

- safety approval as a mandatory binary gate;
- transparent freshness decay;
- explicit topic relevance;
- unique, anti-fraud participation rather than raw volume;
- diversity and author-frequency caps;
- confirmed reports and reversals as negative signals;
- a visible reason code such as `recente`, `tema escolhido` or `participação útil`.

It must not use:

- inferred vulnerability, health, sexuality, religion, politics or economic condition;
- compulsive watch-time optimization;
- hidden emotional manipulation;
- minor data;
- payment to buy organic rank;
- a permanent score attached to a person.

Pulso Credits remain a separate, capped internal-utility ledger. Feed rank must not be purchasable with credits.

## Ghost learning contract

The doctrine remains:

> **data + results = algorithms**

For Pulso, `results` means reviewed outcomes—human moderation decisions, successful appeals, confirmed abuse, user-controlled feedback, safety incidents and measured utility. Raw engagement is not truth and is not a sufficient learning target.

Ghost may initially:

- propose periodic themes;
- summarize consented aggregate signals;
- prioritize review queues;
- generate candidate feed-policy changes;
- explain why a post is eligible for a feed;
- detect drift, abuse patterns and cost anomalies.

Ghost may not autonomously:

- publish, delete or distribute user content;
- ban or penalize a person;
- change credits or feed weights;
- deploy a learned policy;
- copy raw personal content into training data;
- override the safety charter or human appeal.

Every learning change follows:

1. consented and purpose-bound event capture;
2. pseudonymization, retention limits and provenance;
3. offline candidate generation;
4. poisoning and prompt-injection checks;
5. fixed evaluation and red-team suites;
6. human approval;
7. signed/versioned policy release;
8. shadow or canary deployment;
9. drift monitoring, immutable evidence and rollback.

The first alpha keeps Ghost in **shadow mode**: it learns and proposes, but deterministic policy and humans remain authoritative.

## LBArtes safety charter

Pulso and Ghost must preserve these product invariants:

- dignity before engagement;
- explicit consent and purpose limitation;
- minimum data and short retention;
- no child access and no teenage participation in Alpha 0;
- no addictive design as a growth strategy;
- no raw personal-data sale;
- no secret moderation or unappealable punishment;
- explainability, auditability and reversibility;
- safety decisions stay human-accountable;
- costs are bounded before rewards expand;
- growth never silently weakens protection.

## Legal and standards baseline

The shared engineering baseline uses the strict common denominator of:

- Brazil LGPD, ECA Digital and current ANPD age-assurance guidance;
- GDPR data protection by design/default and EDPB age-assurance principles;
- EU Digital Services Act minor-protection guidance;
- UK Online Safety Act/Ofcom child-safety duties;
- US COPPA protections when under-13 data is knowingly collected;
- NIST AI RMF and SSDF;
- OWASP ASVS 5.0 and the OWASP 2025 LLM/GenAI risks.

This is an engineering baseline, not a legal certification. International availability requires country-specific legal review, records of processing, vendor agreements, lawful-basis mapping, retention schedules, incident/reporting obligations and a data-protection impact assessment.

Primary references:

- https://planalto.gov.br/ccivil_03/_ato2023-2026/2025/lei/l15211.htm
- https://www.gov.br/anpd/pt-br/assuntos/eca-digital/
- https://www.gov.br/anpd/pt-br/assuntos/noticias/em-medida-preventiva-anpd-determina-que-discord-suspenda-transmissoes-ao-vivo-no-brasil
- https://www.edpb.europa.eu/our-work-tools/our-documents/statements/statement-12025-age-assurance_en
- https://digital-strategy.ec.europa.eu/en/library/commission-publishes-guidelines-protection-minors
- https://www.ofcom.org.uk/online-safety/protecting-children/protection-of-children-duties-under-the-online-safety-act
- https://www.ftc.gov/legal-library/browse/rules/childrens-online-privacy-protection-rule-coppa
- https://www.nist.gov/itl/ai-risk-management-framework
- https://csrc.nist.gov/projects/ssdf
- https://github.com/OWASP/ASVS
- https://genai.owasp.org/initiatives/top-10-for-llm-and-genai/

## Gates before the first invitation

### P0 — required

- [x] replace self-declared publishing with an adult-verification plus admin-allowlist contract;
- [x] deny and minimize unknown/minor access instead of creating read-only social profiles;
- [x] add durable distributed rate limiting across instances without storing raw IP addresses;
- [x] configure production invite/rate-limit peppers while keeping the runtime Alpha gate closed;
- [x] add explicit trusted-Origin/CSRF rejection to mutating JSON routes;
- [x] require premoderation for every Alpha 0 post;
- [x] add Pulso-specific rules, privacy notice and versioned purpose/consent receipts; focused legal review remains required before invitations;
- [x] add a moderator kill switch and queue-only behavior;
- require MFA for operator access;
- seed one reviewed topic and a small set of reviewed posts;
- execute abuse tests, RLS tests, backup/restore and production smoke checks.

### P1 — before expanding beyond ten people

- [~] append-style operator action log exists; user appeal workflow remains pending;
- retention/deletion jobs with auditable completion;
- privacy-preserving metrics and transparency dashboard;
- incident response and legally reviewed escalation/reporting workflow;
- jurisdiction registry and geo-availability controls;
- external security review and data-protection impact assessment;
- Ghost shadow-learning evaluation suite and signed policy registry.

## Launch language

Allowed:

> Pulso Alpha 0 is a closed adult-only experiment in bounded, explainable and human-accountable social participation.

Not allowed:

- “the safe social network”;
- “compliant worldwide”;
- “AI moderates everything”;
- “Ghost learns by itself in production”;
- “your data earns money”;
- “children and teenagers are protected because they are read-only.”

## Recommended first cohort

- Luiz plus up to nine invited adults;
- one active theme;
- text posts only;
- one moderator/operator window;
- one-week observation period;
- success measured by safety, clarity, useful participation, appeal correctness, repeat intent and cost—not volume or watch time.
