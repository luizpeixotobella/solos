#include "ghostruntime.h"

GhostRuntime::GhostRuntime(QObject *parent)
    : QObject(parent)
    , m_presenceLabel(QStringLiteral("Ghost present in shell"))
    , m_modeLabel(QStringLiteral("Observing · approval-aware · not yet system-bound"))
    , m_thesisLabel(QStringLiteral("Ghost should become a native orchestration layer, not a floating chat pane."))
    , m_intelligenceSummary(QStringLiteral("Layered intelligence pipeline pending runtime data."))
    , m_webStatusLabel(QStringLiteral("Web research not configured"))
    , m_researchQuery(QStringLiteral(""))
    , m_researchSummary(QStringLiteral("No research summary yet."))
    , m_onboardingTitle(QStringLiteral("Brave key onboarding"))
    , m_onboardingBody(QStringLiteral("Ghost should help each SolOS user configure their own Brave key."))
    , m_onboardingUrl(QStringLiteral("https://api-dashboard.search.brave.com/app/keys"))
    , m_onboardingStatus(QStringLiteral("needs-user-key"))
    , m_intentsTitle(QStringLiteral("Ghost intents"))
    , m_intentsSummary(QStringLiteral("Intent routing not loaded yet."))
{
}

QString GhostRuntime::presenceLabel() const
{
    return m_presenceLabel;
}

QString GhostRuntime::modeLabel() const
{
    return m_modeLabel;
}

QString GhostRuntime::thesisLabel() const
{
    return m_thesisLabel;
}

QString GhostRuntime::intelligenceSummary() const
{
    return m_intelligenceSummary;
}

QString GhostRuntime::webStatusLabel() const
{
    return m_webStatusLabel;
}

QString GhostRuntime::researchQuery() const
{
    return m_researchQuery;
}

QString GhostRuntime::researchSummary() const
{
    return m_researchSummary;
}

QString GhostRuntime::onboardingTitle() const
{
    return m_onboardingTitle;
}

QString GhostRuntime::onboardingBody() const
{
    return m_onboardingBody;
}

QString GhostRuntime::onboardingUrl() const
{
    return m_onboardingUrl;
}

QString GhostRuntime::onboardingStatus() const
{
    return m_onboardingStatus;
}

QString GhostRuntime::intentsTitle() const
{
    return m_intentsTitle;
}

QString GhostRuntime::intentsSummary() const
{
    return m_intentsSummary;
}

QStringList GhostRuntime::intentLines() const
{
    return m_intentLines;
}

QStringList GhostRuntime::pipelineLines() const
{
    return m_pipelineLines;
}

QStringList GhostRuntime::citationLines() const
{
    return m_citationLines;
}

void GhostRuntime::setLabels(const QString &presence,
                             const QString &mode,
                             const QString &thesis,
                             const QString &intelligenceSummary,
                             const QString &webStatusLabel,
                             const QString &researchQuery,
                             const QString &researchSummary,
                             const QString &onboardingTitle,
                             const QString &onboardingBody,
                             const QString &onboardingUrl,
                             const QString &onboardingStatus,
                             const QString &intentsTitle,
                             const QString &intentsSummary,
                             const QStringList &intentLines,
                             const QStringList &pipelineLines,
                             const QStringList &citationLines)
{
    m_presenceLabel = presence;
    m_modeLabel = mode;
    m_thesisLabel = thesis;
    m_intelligenceSummary = intelligenceSummary;
    m_webStatusLabel = webStatusLabel;
    m_researchQuery = researchQuery;
    m_researchSummary = researchSummary;
    m_onboardingTitle = onboardingTitle;
    m_onboardingBody = onboardingBody;
    m_onboardingUrl = onboardingUrl;
    m_onboardingStatus = onboardingStatus;
    m_intentsTitle = intentsTitle;
    m_intentsSummary = intentsSummary;
    m_intentLines = intentLines;
    m_pipelineLines = pipelineLines;
    m_citationLines = citationLines;
    emit stateChanged();
}
