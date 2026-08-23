# Ghost Audit Pilot — targeted fundraising campaign

Date: 2026-08-23

Status: launched publicly on 2026-08-23 after the release commit and GitHub issue form became public.

## Publication registry

- Campaign article: https://luiz-bella-artes.net/blog/solos-ghost-audit-pilot-rc2-dez-auditores
- LinkedIn detailed invitation: https://www.linkedin.com/feed/update/urn:li:share:7497427122525724672
- LinkedIn Ghost video: https://www.linkedin.com/feed/update/urn:li:ugcPost:7497430783851294720/
- X Ghost video: https://x.com/LbartesL/status/2091665940576387181
- Instagram Ghost Reel: https://www.instagram.com/lbiartesluiz/reel/DcZqwRBkWdh/
- Manifesto amplification:
  - LinkedIn: https://www.linkedin.com/feed/update/urn:li:ugcPost:7497429367451656192/
  - X: https://x.com/LbartesL/status/2091666693885923615
  - Instagram: https://www.instagram.com/lbiartesluiz/reel/DcZsDKXk-7W/
- TikTok and Kwai were not marked published because both upload surfaces require a fresh authenticated session.

GitHub traffic at launch was 24 clones and 17 unique cloners in the rolling 14-day window. This remains an acquisition signal, not a completed audit, identified lead or customer count.

## Audience

Start with people already showing technical intent: repository cloners, developers interested in agent safety, local-first AI and auditable runtime design. Do not lead with a broad donation request.

## Exchange before the ask

Each of the first ten reviewers gets:

- a one-command audit they run on their own Linux machine;
- their own exact input classification and transparent route;
- an approval/denial test;
- a real isolated artifact plus a portable verifier receipt;
- pilot acknowledgement if they explicitly consent;
- early access to the next Ghost audit iteration and the existing Founder Heart utility path under its published non-financial terms.

The project gets:

- a reproducible receipt;
- a human classification verdict;
- second-run/return intent;
- a concrete support-level signal;
- bug and classification cases that can improve the next evaluation seed.

## Primary Portuguese invitation

> Estou procurando 10 pessoas — não 10 curtidas — para tentar quebrar uma função nova do Ghost/SolOS.
>
> Você clona o projeto, entrega qualquer texto ao Ghost e vê quatro coisas: como ele classifica o risco, se mantém instruções perigosas inertes, se para para pedir aprovação e se um executável separado consegue verificar o artefato por hash.
>
> Você sai com um recibo local reproduzível. Eu saio com o seu veredito humano: a classificação foi útil? Você rodaria uma segunda auditoria? Isso vale apoio financeiro concreto ou ainda não?
>
> São 10 vagas porque quero medir retorno real antes de fazer propaganda maior. O teste não executa comandos do seu texto, não pede chave, senha ou carteira e não afirma verificar a verdade da internet.
>
> Repositório: https://github.com/luizpeixotobella/solos
> Instrução: `./tools/ghost-audit-pilot.sh "seu input"`

## Short WhatsApp version

> Preciso de 10 pessoas para auditar o Ghost de verdade. Você clona, envia um input seu, testa aprovação/recusa e recebe um recibo verificável por hash. Não é chatbot de vitrine e o texto nunca vira comando. Em troca, preciso do seu veredito: acertou a classificação, você usaria de novo e isso já vale algum apoio? https://github.com/luizpeixotobella/solos

## Support ask after a passing receipt

The support question comes only after the reviewer has received value:

> Você já viu o Ghost funcionar e levou um recibo verificável. Qual destas respostas é honesta hoje?
>
> - R$ 10 para manter o kit público;
> - R$ 25 pelo caminho Founder Heart e acesso antecipado ao piloto;
> - R$ 50 com reconhecimento de apoiador-piloto, se você consentir;
> - eu testaria novamente, mas ainda não pagaria;
> - ainda não existe valor suficiente.

This is a product-signal question, not pressure and not an investment offer.

## Funnel

1. Acquisition: GitHub clone or direct repository visit.
2. Activation: `ghost-audit-pilot.sh` reaches `awaiting-approval`.
3. Value: receipt reaches `status=passed` or demonstrates a deliberate fail-closed result.
4. Human validation: reviewer submits the GitHub issue form.
5. Retention: reviewer runs a second input within seven days.
6. Support: reviewer selects a concrete contribution level and, separately, may use the public fundraiser path.

Do not combine clone count, receipt count and payment count. They are different funnel stages.

## Campaign stop/go rule

- Go to a larger campaign only after 10 valid reviews, at least 7 useful classifications, 5 second-run intentions, 3 actual seven-day returns and 2 concrete support signals.
- Improve and rerun the pilot if usefulness is below 7/10.
- Change the offer if people return but will not support.
- Stop claiming this direction has product pull if reviewers neither return nor see value.

## Forbidden claims

- “Ghost verifies any fact automatically.”
- “Ghost executes any input autonomously.”
- “The receipt is a security certification.”
- “17 unique cloners means 17 people/customers.”
- Any profit, yield, equity, guaranteed return or cash-redemption language.

The strongest valid claim is:

> Ghost now accepts a real input, exposes a transparent deterministic safety route, keeps embedded instructions inert, waits for approval, creates one isolated Linux proof and binds a separate verifier receipt to the retained hashes.
