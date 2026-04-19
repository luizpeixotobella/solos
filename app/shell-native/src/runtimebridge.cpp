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
        object.value(QStringLiteral("title")).toString(),
        object.value(QStringLiteral("scope")).toString(),
        object.value(QStringLiteral("risk")).toString()
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

    snapshot.sessionLabel = root.value(QStringLiteral("sessionLabel")).toString();
    snapshot.systemLabel = root.value(QStringLiteral("systemLabel")).toString();
    snapshot.walletLabel = root.value(QStringLiteral("walletLabel")).toString();
    snapshot.agentStatus = root.value(QStringLiteral("agentStatus")).toString();

    snapshot.summaryTitle = home.value(QStringLiteral("summaryTitle")).toString();
    snapshot.summarySubtitle = home.value(QStringLiteral("summarySubtitle")).toString();
    snapshot.summaryBody = home.value(QStringLiteral("summaryBody")).toString();
    snapshot.nextActionTitle = home.value(QStringLiteral("nextActionTitle")).toString();
    snapshot.nextActionSubtitle = home.value(QStringLiteral("nextActionSubtitle")).toString();
    snapshot.nextActionBody = home.value(QStringLiteral("nextActionBody")).toString();

    snapshot.ghostPresenceLabel = ghost.value(QStringLiteral("presenceLabel")).toString();
    snapshot.ghostModeLabel = ghost.value(QStringLiteral("modeLabel")).toString();
    snapshot.ghostThesisLabel = ghost.value(QStringLiteral("thesisLabel")).toString();

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
