use crate::event_ledger::DaemonEvent;
use serde::Serialize;

pub const GHOST_AUTONOMY_SCHEMA: &str = "solos.ghost.autonomy-governor.v1";

/// Signals used by the autonomy governor. They are counts and capability facts,
/// never raw prompts or personal payloads.
#[derive(Debug, Clone, Default, PartialEq)]
pub struct AutonomySignals {
    pub event_count: usize,
    pub trace_count: usize,
    pub verified_resolutions: usize,
    pub verified_audits: usize,
    pub denied_decisions: usize,
    pub grounding_available: bool,
    pub approval_lane_available: bool,
}

#[derive(Debug, Clone, Serialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct GhostAutonomyGovernor {
    pub schema: String,
    pub title: String,
    pub maxim: String,
    pub previous_maxim: String,
    pub mode: String,
    pub level: String,
    pub summary: String,
    pub evidence_count: usize,
    pub verified_outcomes: usize,
    pub denied_decisions: usize,
    pub lanes: Vec<AutonomyLane>,
    pub next_gate: String,
    pub rollback_policy: String,
}

#[derive(Debug, Clone, Serialize, PartialEq)]
#[serde(rename_all = "camelCase")]
pub struct AutonomyLane {
    pub name: String,
    pub status: String,
    pub boundary: String,
    pub evidence: String,
}

/// Derive a bounded autonomy state from durable operational events.
///
/// The deliberate rule is that evidence may unlock a more capable *proposal*
/// lane, but no event count can unlock unattended writes, payments, public
/// posts, shell commands, or network actions.
pub fn derive(signals: &AutonomySignals) -> GhostAutonomyGovernor {
    let verified_outcomes = signals.verified_resolutions + signals.verified_audits;
    let level = if verified_outcomes >= 2 && signals.trace_count >= 1 {
        "approval-bound"
    } else if verified_outcomes >= 1 || signals.trace_count >= 1 {
        "propose-with-evidence"
    } else {
        "observe"
    };

    let evidence_count = signals
        .event_count
        .saturating_add(signals.trace_count)
        .saturating_add(verified_outcomes);
    let verified_note = if verified_outcomes == 0 {
        "nenhum outcome verificado ainda"
    } else {
        "outcomes verificados preservados no ledger"
    };
    let denial_note = if signals.denied_decisions == 0 {
        "nenhuma decisão negada registrada"
    } else {
        "negações permanecem freio de segurança, não erro a esconder"
    };

    GhostAutonomyGovernor {
        schema: GHOST_AUTONOMY_SCHEMA.into(),
        title: "Ghost autonomy governor · Just Intelligent".into(),
        maxim: "Just Intelligent (JI): inteligência mimetizada no fluxo, na medida certa e no momento certo.".into(),
        previous_maxim: "Just in Time (JIT): fazer no momento certo.".into(),
        mode: "mimetized-operating-intelligence".into(),
        level: level.into(),
        summary: format!(
            "Ghost avançou para {level}: observa evidência, aprende o limite da rota e propõe a próxima ação sem transformar confiança em permissão. {verified_note}; {denial_note}."
        ),
        evidence_count,
        verified_outcomes,
        denied_decisions: signals.denied_decisions,
        lanes: vec![
            AutonomyLane {
                name: "observe-local".into(),
                status: "active".into(),
                boundary: "Fatos locais, documentação e ledger minimizado.".into(),
                evidence: format!("{} evento(s) operacional(is) considerados.", signals.event_count),
            },
            AutonomyLane {
                name: "propose-route".into(),
                status: if level == "observe" { "waiting-for-evidence" } else { "active" }.into(),
                boundary: "Classificar, explicar e preparar plano; não produzir efeito externo.".into(),
                evidence: format!("{} trace(s), grounding {}.", signals.trace_count, if signals.grounding_available { "disponível" } else { "indisponível" }),
            },
            AutonomyLane {
                name: "approval-bound-action".into(),
                status: if signals.approval_lane_available { "gated" } else { "unavailable" }.into(),
                boundary: "Escrita, rede, wallet, shell e publicação exigem aprovação explícita.".into(),
                evidence: format!("{} outcome(s) verificado(s); approval lane {}.", verified_outcomes, if signals.approval_lane_available { "visível" } else { "não disponível" }),
            },
            AutonomyLane {
                name: "verify-and-learn".into(),
                status: "active".into(),
                boundary: "Verificar resultado; feedback humano aceito/rejeitado/corrigido vira avaliação separada.".into(),
                evidence: format!("{} auditoria(s) verificadas; rollback sempre permitido.", signals.verified_audits),
            },
        ],
        next_gate: if verified_outcomes >= 2 && signals.trace_count >= 1 {
            "Coletar avaliação humana suficiente e autorizar uma capability estreita, com quota, expiração e rollback.".into()
        } else {
            "Aumentar traces e outcomes verificáveis antes de ampliar qualquer capability.".into()
        },
        rollback_policy: "Qualquer regressão, negação ou resultado não verificado mantém a rota em aprovação ou volta para observe; autonomia nunca revoga o limite humano.".into(),
    }
}

/// Convert only event metadata/status into safe governor signals.
pub fn signals_from_events(
    events: &[DaemonEvent],
    grounding_available: bool,
    approval_lane_available: bool,
) -> AutonomySignals {
    let mut signals = AutonomySignals {
        event_count: events.len(),
        grounding_available,
        approval_lane_available,
        ..AutonomySignals::default()
    };

    for event in events {
        match event.kind.as_str() {
            "ghost.trace" | "ghost.resolution.selected" | "ghost.resolution.awaiting-approval" => {
                signals.trace_count += 1;
            }
            "ghost.resolution.decided" => {
                if event
                    .data
                    .pointer("/result/status")
                    .and_then(|value| value.as_str())
                    == Some("resolved")
                {
                    signals.verified_resolutions += 1;
                } else {
                    signals.denied_decisions += 1;
                }
            }
            "ghost.audit.verified" => signals.verified_audits += 1,
            "ghost.audit.decided"
                if event
                    .data
                    .pointer("/result/approved")
                    .and_then(|value| value.as_bool())
                    == Some(false) =>
            {
                signals.denied_decisions += 1;
            }
            _ => {}
        }
    }

    signals
}

#[cfg(test)]
mod tests {
    use super::*;
    use serde_json::json;

    fn event(kind: &str, data: serde_json::Value) -> DaemonEvent {
        DaemonEvent {
            schema: "solos.daemon.event.v2".into(),
            event_id: format!("test-{kind}"),
            sequence: 1,
            kind: kind.into(),
            timestamp: 1,
            data,
        }
    }

    #[test]
    fn starts_in_observe_without_evidence() {
        let governor = derive(&AutonomySignals::default());
        assert_eq!(governor.level, "observe");
        assert_eq!(governor.lanes[2].status, "unavailable");
    }

    #[test]
    fn verified_outcomes_unlock_proposal_but_not_unattended_action() {
        let signals = AutonomySignals {
            trace_count: 1,
            verified_resolutions: 1,
            approval_lane_available: true,
            ..AutonomySignals::default()
        };
        let governor = derive(&signals);
        assert_eq!(governor.level, "propose-with-evidence");
        assert_eq!(governor.lanes[2].status, "gated");
        assert!(governor.lanes[2].boundary.contains("aprovação explícita"));
    }

    #[test]
    fn event_projection_keeps_denials_as_safety_signal() {
        let signals = signals_from_events(
            &[
                event(
                    "ghost.resolution.decided",
                    json!({"result": {"status": "blocked"}}),
                ),
                event(
                    "ghost.audit.decided",
                    json!({"result": {"approved": false}}),
                ),
            ],
            false,
            true,
        );
        assert_eq!(signals.denied_decisions, 2);
        assert_eq!(signals.verified_resolutions, 0);
        assert_eq!(signals.approval_lane_available, true);
    }
}
