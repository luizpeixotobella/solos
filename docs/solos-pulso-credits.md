# SolOS Pulso and Pulso Credits

Date: 2026-07-14

Status: product/economic direction for a future SolOS app surface.

## What SolOS Pulso is

SolOS Pulso is a planned SolOS-integrated social surface for consented human signals.

It should start as:

- a closed SolOS/Heart Pass-aware pilot;
- a feed for text, video, comments, likes, and topic responses;
- a clean one-word challenge format for cultural signals;
- a future SolOS Apps module, not a disconnected generic social network;
- a source of aggregate context for Ghost, never a silent raw-data extraction layer.

The first public preview lives in the CMS at:

- `/solos/pulso`

That preview is read-only. It must not collect data, track clicks, or mutate user state.

## Why this belongs in SolOS

Pulso fits SolOS when it is mediated by the operating layer:

- Apps exposes Pulso as a module.
- Ghost proposes themes and summarizes aggregate signal patterns.
- Wallet can show Pulso Credits and participation benefits.
- Approvals govern consent and sensitive data use.
- Identity/Heart Pass can gate pilot access and paid utility.

Pulso should not bypass the SolOS safety thesis. It should demonstrate it.

## Pulso Credits

Pulso Credits are the proposed first return mechanism for useful participation.

They are:

- internal utility credits;
- earned from quality signals, not volume alone;
- bounded by monthly caps and anti-fraud rules;
- redeemable for SolOS/Ghost utility, upload allowance, access, tools, discounts, or later creator-pool participation;
- not cash, yield, passive income, or guaranteed revenue share.

Core principle:

> The user is not rewarded because they gave data. The user is rewarded when their participation helps the network create useful, consented, auditable context.

## Cost guardrails

Pulso cannot let video, AI, search, or storage costs grow without a matching revenue path.

Initial guardrails:

- free tier has strict upload/video limits;
- heavy media requires paid plan, Heart Pass utility, sponsor budget, or earned quota;
- Ghost/AI usage consumes quota or Pulso Credits;
- low-quality repeated activity earns zero;
- suspicious activity can be penalized;
- credits can expire;
- creator pool or cash-like redemption waits for revenue, legal review, tax treatment, and accounting.

## Signal weighting

Early weighting should be conservative:

- isolated click: weak, very low or zero credit;
- 3-second video view: weak-to-medium;
- valid one-word answer: medium, clean cultural signal;
- useful comment: stronger context;
- original video/post with verified engagement: stronger;
- spam or artificial engagement: zero or penalty.

## Product path

1. Keep the CMS public preview read-only.
2. Add persistent Supabase tables for posts, responses, events, and credit ledger.
3. Gate real pilot access with authenticated SolOS identity.
4. Add consent, export, deletion, moderation, and anti-fraud.
5. Surface Pulso as a planned module in SolOS Apps.
6. Let Wallet expose Pulso Credits only after ledger semantics are real.
7. Let Ghost propose topics and summarize aggregate patterns only after consent rules are live.

## Non-promises

Do not claim:

- financial return;
- guaranteed creator earnings;
- yield;
- passive income;
- public user capture before consent;
- that Pulso Credits are redeemable for cash.

Correct framing:

> Pulso Credits are an internal utility mechanism for returning value inside SolOS before any legally reviewed creator-pool or revenue-share model exists.

