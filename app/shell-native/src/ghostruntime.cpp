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
    , m_initiationStatus(QStringLiteral("waiting"))
    , m_initiationSummary(QStringLiteral("Ghost initiation has not started yet."))
    , m_initiationDatabasePath(QStringLiteral("config/ghost_knowledge.json"))
    , m_languageSupportStatus(QStringLiteral("planned-core-capability"))
    , m_languageSupportSummary(QStringLiteral("Ghost multilingual support is planned as an operating capability, not just UI localization."))
    , m_readinessStatus(QStringLiteral("foundation"))
    , m_readinessSummary(QStringLiteral("Ghost operational readiness has not been loaded yet."))
    , m_requestClassifierTitle(QStringLiteral("Ghost request classifier"))
    , m_requestClassifierSummary(QStringLiteral("Request classification has not been loaded yet."))
    , m_actionTraceSummary(QStringLiteral("No Ghost action trace loaded yet."))
    , m_routeExplanationSummary(QStringLiteral("No Ghost route explanation loaded yet."))
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

QString GhostRuntime::initiationStatus() const
{
    return m_initiationStatus;
}

QString GhostRuntime::initiationSummary() const
{
    return m_initiationSummary;
}

QString GhostRuntime::initiationDatabasePath() const
{
    return m_initiationDatabasePath;
}

QStringList GhostRuntime::knowledgeLines() const
{
    return m_knowledgeLines;
}

QString GhostRuntime::languageSupportStatus() const
{
    return m_languageSupportStatus;
}

QString GhostRuntime::languageSupportSummary() const
{
    return m_languageSupportSummary;
}

QStringList GhostRuntime::languageSupportLines() const
{
    return m_languageSupportLines;
}

QString GhostRuntime::readinessStatus() const
{
    return m_readinessStatus;
}

QString GhostRuntime::readinessSummary() const
{
    return m_readinessSummary;
}

QStringList GhostRuntime::readinessLines() const
{
    return m_readinessLines;
}

QString GhostRuntime::requestClassifierTitle() const
{
    return m_requestClassifierTitle;
}

QString GhostRuntime::requestClassifierSummary() const
{
    return m_requestClassifierSummary;
}

QStringList GhostRuntime::requestClassificationLines() const
{
    return m_requestClassificationLines;
}

QString GhostRuntime::actionTraceSummary() const
{
    return m_actionTraceSummary;
}

QString GhostRuntime::routeExplanationSummary() const
{
    return m_routeExplanationSummary;
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
                             const QStringList &citationLines,
                             const QString &initiationStatus,
                             const QString &initiationSummary,
                             const QString &initiationDatabasePath,
                             const QStringList &knowledgeLines,
                             const QString &languageSupportStatus,
                             const QString &languageSupportSummary,
                             const QStringList &languageSupportLines,
                             const QString &readinessStatus,
                             const QString &readinessSummary,
                             const QStringList &readinessLines,
                             const QString &requestClassifierTitle,
                             const QString &requestClassifierSummary,
                             const QStringList &requestClassificationLines,
                             const QString &actionTraceSummary,
                             const QString &routeExplanationSummary)
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
    m_initiationStatus = initiationStatus;
    m_initiationSummary = initiationSummary;
    m_initiationDatabasePath = initiationDatabasePath;
    m_knowledgeLines = knowledgeLines;
    m_languageSupportStatus = languageSupportStatus;
    m_languageSupportSummary = languageSupportSummary;
    m_languageSupportLines = languageSupportLines;
    m_readinessStatus = readinessStatus;
    m_readinessSummary = readinessSummary;
    m_readinessLines = readinessLines;
    m_requestClassifierTitle = requestClassifierTitle;
    m_requestClassifierSummary = requestClassifierSummary;
    m_requestClassificationLines = requestClassificationLines;
    m_actionTraceSummary = actionTraceSummary;
    m_routeExplanationSummary = routeExplanationSummary;
    emit stateChanged();
}
