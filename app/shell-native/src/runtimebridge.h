#pragma once

#include <QString>
#include <QVector>

#include "activityfeedmodel.h"
#include "appregistrymodel.h"
#include "approvalqueuemodel.h"
#include "ghostruntime.h"
#include "homestate.h"
#include "quickactionsmodel.h"

struct RuntimeSnapshotData {
    QString sessionLabel;
    QString systemLabel;
    QString walletLabel;
    QString agentStatus;
    QString runtimeMode;
    QString runtimeSource;
    QString runtimeRole;
    QString mediationStatus;
    QString summaryTitle;
    QString summarySubtitle;
    QString summaryBody;
    QString nextActionTitle;
    QString nextActionSubtitle;
    QString nextActionBody;
    QString ghostPresenceLabel;
    QString ghostModeLabel;
    QString ghostThesisLabel;
    QString hostRuntimeSummary;
    bool online = false;
    int approvalsCount = 0;
    int notificationsCount = 0;
    QVector<QuickActionEntry> quickActions;
    QVector<ActivityFeedEntry> activityFeed;
    QVector<ApprovalQueueEntry> approvals;
    QVector<AppRegistryEntry> apps;
    bool isValid = false;
};

class RuntimeBridge
{
public:
    static RuntimeSnapshotData loadSnapshot(const QString &path);
};
