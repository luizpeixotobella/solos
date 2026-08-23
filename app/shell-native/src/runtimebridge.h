#pragma once

#include <QString>
#include <QStringList>
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
    QString ghostIntelligenceSummary;
    QString ghostWebStatusLabel;
    QString ghostResearchQuery;
    QString ghostResearchSummary;
    QString ghostOnboardingTitle;
    QString ghostOnboardingBody;
    QString ghostOnboardingUrl;
    QString ghostOnboardingStatus;
    QString ghostIntentsTitle;
    QString ghostIntentsSummary;
    QStringList ghostIntentLines;
    QStringList ghostPipelineLines;
    QStringList ghostCitationLines;
    QString ghostInitiationStatus;
    QString ghostInitiationSummary;
    QString ghostInitiationDatabasePath;
    QStringList ghostKnowledgeLines;
    QString ghostLanguageSupportStatus;
    QString ghostLanguageSupportSummary;
    QStringList ghostLanguageSupportLines;
    QString ghostReadinessStatus;
    QString ghostReadinessSummary;
    QStringList ghostReadinessLines;
    QString ghostRequestClassifierTitle;
    QString ghostRequestClassifierSummary;
    QStringList ghostRequestClassificationLines;
    QString ghostActionTraceSummary;
    QString ghostRouteExplanationSummary;
    QString ghostResolutionStatus;
    QString ghostResolutionSummary;
    QString ghostResolutionSelectedId;
    QString ghostResolutionCurrentStep;
    int ghostResolutionProgress = 0;
    QStringList ghostResolutionLines;
    QString ghostAuditStatus;
    QString ghostAuditSummary;
    QString ghostAuditActiveId;
    QString ghostAuditInput;
    QString ghostAuditInputSha256;
    QString ghostAuditRequestClass;
    QString ghostAuditRisk;
    QString ghostAuditRoute;
    QString ghostAuditCurrentStep;
    int ghostAuditProgress = 0;
    QString ghostAuditArtifactPath;
    QString ghostAuditReceiptPath;
    QStringList ghostAuditLines;
    QString heartPassTitle;
    QString heartPassStatus;
    QString heartPassNetwork;
    QString heartPassTokenStandard;
    QString heartPassContract;
    QString heartPassTokenId;
    QString heartPassOpenSeaUrl;
    QString heartPassSummary;
    QString heartPassNextStep;
    QString heartPassWalletAddress;
    QString heartPassOwnerAddress;
    QString heartPassVerificationStatus;
    QString heartPassLastCheckedAt;
    QString heartPassConfigPath;
    QStringList heartPassCapabilityLines;
    QString heartPassQuotaTitle;
    QString heartPassQuotaStatus;
    QString heartPassQuotaMode;
    QString heartPassQuotaPeriod;
    int heartPassQuotaIncludedQueries = 0;
    int heartPassQuotaUsedQueries = 0;
    int heartPassQuotaRemainingQueries = 0;
    QString heartPassQuotaFallback;
    QString heartPassQuotaUsageSource;
    QString heartPassQuotaLastSync;
    QString heartPassQuotaResetPolicy;
    QString heartPassQuotaSummary;
    QString heartPassQuotaNextStep;
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
    static RuntimeSnapshotData loadSnapshotFromDaemon(const QString &socketPath, int timeoutMs = 350);
    static RuntimeSnapshotData parseSnapshot(const QByteArray &payload);
};
