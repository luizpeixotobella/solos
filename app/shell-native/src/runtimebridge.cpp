#include "runtimebridge.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace {
QuickActionEntry parseQuickAction(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("title")).toString(),
        object.value(QStringLiteral("subtitle")).toString(),
        object.value(QStringLiteral("description")).toString()
    };
}

ActivityFeedEntry parseActivityEntry(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("title")).toString(),
        object.value(QStringLiteral("detail")).toString(),
        object.value(QStringLiteral("status")).toString()
    };
}

ApprovalQueueEntry parseApprovalEntry(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("id")).toString(),
        object.value(QStringLiteral("title")).toString(),
        object.value(QStringLiteral("description")).toString(),
        object.value(QStringLiteral("requestedBy")).toString(),
        object.value(QStringLiteral("capability")).toString(),
        object.value(QStringLiteral("scope")).toString(),
        object.value(QStringLiteral("risk")).toString(),
        object.value(QStringLiteral("status")).toString(),
        object.value(QStringLiteral("createdAt")).toString()
    };
}

AppRegistryEntry parseAppEntry(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("name")).toString(),
        object.value(QStringLiteral("subtitle")).toString(),
        object.value(QStringLiteral("description")).toString()
    };
}

QStringList parseGhostPipelineLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString name = object.value(QStringLiteral("name")).toString();
        const QString status = object.value(QStringLiteral("status")).toString();
        const QString detail = object.value(QStringLiteral("detail")).toString();
        lines.append(QStringLiteral("%1 [%2] %3").arg(name, status, detail));
    }
    return lines;
}

QStringList parseGhostCitationLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString title = object.value(QStringLiteral("title")).toString();
        const QString url = object.value(QStringLiteral("url")).toString();
        const QString snippet = object.value(QStringLiteral("snippet")).toString();
        lines.append(QStringLiteral("%1\n%2\n%3").arg(title, url, snippet));
    }
    return lines;
}
}

RuntimeSnapshotData RuntimeBridge::loadSnapshot(const QString &path)
{
    RuntimeSnapshotData snapshot;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return snapshot;
    }

    const auto document = QJsonDocument::fromJson(file.readAll());
    if (!document.isObject()) {
        return snapshot;
    }

    const QJsonObject root = document.object();
    const QJsonObject home = root.value(QStringLiteral("home")).toObject();
    const QJsonObject ghost = root.value(QStringLiteral("ghost")).toObject();
    const QJsonObject systemStatus = root.value(QStringLiteral("systemStatus")).toObject();
    const QJsonObject lastResearch = ghost.value(QStringLiteral("lastResearch")).toObject();

    snapshot.sessionLabel = root.value(QStringLiteral("sessionLabel")).toString();
    snapshot.systemLabel = root.value(QStringLiteral("systemLabel")).toString();
    snapshot.walletLabel = root.value(QStringLiteral("walletLabel")).toString();
    snapshot.agentStatus = root.value(QStringLiteral("agentStatus")).toString();
    snapshot.runtimeMode = root.value(QStringLiteral("runtimeMode")).toString();
    snapshot.runtimeSource = root.value(QStringLiteral("runtimeSource")).toString();
    snapshot.runtimeRole = root.value(QStringLiteral("runtimeRole")).toString();
    snapshot.mediationStatus = root.value(QStringLiteral("mediationStatus")).toString();

    snapshot.summaryTitle = home.value(QStringLiteral("summaryTitle")).toString();
    snapshot.summarySubtitle = home.value(QStringLiteral("summarySubtitle")).toString();
    snapshot.summaryBody = home.value(QStringLiteral("summaryBody")).toString();
    snapshot.nextActionTitle = home.value(QStringLiteral("nextActionTitle")).toString();
    snapshot.nextActionSubtitle = home.value(QStringLiteral("nextActionSubtitle")).toString();
    snapshot.nextActionBody = home.value(QStringLiteral("nextActionBody")).toString();

    snapshot.ghostPresenceLabel = ghost.value(QStringLiteral("presenceLabel")).toString();
    snapshot.ghostModeLabel = ghost.value(QStringLiteral("modeLabel")).toString();
    snapshot.ghostThesisLabel = ghost.value(QStringLiteral("thesisLabel")).toString();
    snapshot.ghostIntelligenceSummary = ghost.value(QStringLiteral("intelligenceSummary")).toString();
    snapshot.ghostWebStatusLabel = ghost.value(QStringLiteral("webStatusLabel")).toString();
    snapshot.ghostResearchQuery = lastResearch.value(QStringLiteral("query")).toString();
    snapshot.ghostResearchSummary = lastResearch.value(QStringLiteral("summary")).toString();
    snapshot.ghostOnboardingTitle = ghost.value(QStringLiteral("onboardingTitle")).toString();
    snapshot.ghostOnboardingBody = ghost.value(QStringLiteral("onboardingBody")).toString();
    snapshot.ghostOnboardingUrl = ghost.value(QStringLiteral("onboardingUrl")).toString();
    snapshot.ghostOnboardingStatus = ghost.value(QStringLiteral("onboardingStatus")).toString();
    snapshot.ghostPipelineLines = parseGhostPipelineLines(ghost.value(QStringLiteral("pipelineStages")).toArray());
    snapshot.ghostCitationLines = parseGhostCitationLines(lastResearch.value(QStringLiteral("citations")).toArray());
    snapshot.hostRuntimeSummary = systemStatus.value(QStringLiteral("hostRuntimeSummary")).toString();
    snapshot.online = systemStatus.value(QStringLiteral("online")).toBool(false);
    snapshot.approvalsCount = systemStatus.value(QStringLiteral("approvalsCount")).toInt();
    snapshot.notificationsCount = systemStatus.value(QStringLiteral("notificationsCount")).toInt();

    const QJsonArray quickActions = root.value(QStringLiteral("quickActions")).toArray();
    for (const QJsonValue &value : quickActions) {
        snapshot.quickActions.append(parseQuickAction(value.toObject()));
    }

    const QJsonArray activityFeed = root.value(QStringLiteral("activityFeed")).toArray();
    for (const QJsonValue &value : activityFeed) {
        snapshot.activityFeed.append(parseActivityEntry(value.toObject()));
    }

    const QJsonArray approvals = root.value(QStringLiteral("approvals")).toArray();
    for (const QJsonValue &value : approvals) {
        snapshot.approvals.append(parseApprovalEntry(value.toObject()));
    }

    const QJsonArray apps = root.value(QStringLiteral("apps")).toArray();
    for (const QJsonValue &value : apps) {
        snapshot.apps.append(parseAppEntry(value.toObject()));
    }

    snapshot.isValid = !snapshot.sessionLabel.isEmpty();
    return snapshot;
}
