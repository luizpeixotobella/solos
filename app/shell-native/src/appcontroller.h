#pragma once

#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include "activityfeedmodel.h"
#include "appregistrymodel.h"
#include "approvalqueuemodel.h"
#include "ghostruntime.h"
#include "homestate.h"
#include "quickactionsmodel.h"

class AppController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentScreen READ currentScreen WRITE setCurrentScreen NOTIFY currentScreenChanged)
    Q_PROPERTY(QString sessionLabel READ sessionLabel NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString systemLabel READ systemLabel NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString walletLabel READ walletLabel NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString agentStatus READ agentStatus NOTIFY runtimeStateChanged)
    Q_PROPERTY(HomeState* homeState READ homeState CONSTANT)
    Q_PROPERTY(QStringList appNames READ appNames CONSTANT)
    Q_PROPERTY(AppRegistryModel* appRegistryModel READ appRegistryModel CONSTANT)
    Q_PROPERTY(ActivityFeedModel* activityFeedModel READ activityFeedModel CONSTANT)
    Q_PROPERTY(QuickActionsModel* quickActionsModel READ quickActionsModel CONSTANT)
    Q_PROPERTY(ApprovalQueueModel* approvalQueueModel READ approvalQueueModel CONSTANT)
    Q_PROPERTY(GhostRuntime* ghostRuntime READ ghostRuntime CONSTANT)
    Q_PROPERTY(QString runtimeStatus READ runtimeStatus NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString runtimeSource READ runtimeSource NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString hostRuntimeSummary READ hostRuntimeSummary NOTIFY runtimeStateChanged)
    Q_PROPERTY(bool online READ online NOTIFY runtimeStateChanged)
    Q_PROPERTY(int approvalsCount READ approvalsCount NOTIFY runtimeStateChanged)
    Q_PROPERTY(int notificationsCount READ notificationsCount NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString lastRuntimeRefresh READ lastRuntimeRefresh NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString ghostConfigStatus READ ghostConfigStatus NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassTitle READ heartPassTitle NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassStatus READ heartPassStatus NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassNetwork READ heartPassNetwork NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassTokenStandard READ heartPassTokenStandard NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassContract READ heartPassContract NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassTokenId READ heartPassTokenId NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassOpenSeaUrl READ heartPassOpenSeaUrl NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassSummary READ heartPassSummary NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassNextStep READ heartPassNextStep NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassWalletAddress READ heartPassWalletAddress NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassOwnerAddress READ heartPassOwnerAddress NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassVerificationStatus READ heartPassVerificationStatus NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassLastCheckedAt READ heartPassLastCheckedAt NOTIFY runtimeStateChanged)
    Q_PROPERTY(QString heartPassConfigPath READ heartPassConfigPath NOTIFY runtimeStateChanged)
    Q_PROPERTY(QStringList heartPassCapabilityLines READ heartPassCapabilityLines NOTIFY runtimeStateChanged)

public:
    explicit AppController(QObject *parent = nullptr);

    QString currentScreen() const;
    void setCurrentScreen(const QString &screen);

    QString sessionLabel() const;
    QString systemLabel() const;
    QString walletLabel() const;
    QString agentStatus() const;
    HomeState *homeState();
    QStringList appNames() const;
    AppRegistryModel *appRegistryModel();
    ActivityFeedModel *activityFeedModel();
    QuickActionsModel *quickActionsModel();
    ApprovalQueueModel *approvalQueueModel();
    GhostRuntime *ghostRuntime();
    QString runtimeStatus() const;
    QString runtimeSource() const;
    QString hostRuntimeSummary() const;
    bool online() const;
    int approvalsCount() const;
    int notificationsCount() const;
    QString lastRuntimeRefresh() const;
    QString ghostConfigStatus() const;
    QString heartPassTitle() const;
    QString heartPassStatus() const;
    QString heartPassNetwork() const;
    QString heartPassTokenStandard() const;
    QString heartPassContract() const;
    QString heartPassTokenId() const;
    QString heartPassOpenSeaUrl() const;
    QString heartPassSummary() const;
    QString heartPassNextStep() const;
    QString heartPassWalletAddress() const;
    QString heartPassOwnerAddress() const;
    QString heartPassVerificationStatus() const;
    QString heartPassLastCheckedAt() const;
    QString heartPassConfigPath() const;
    QStringList heartPassCapabilityLines() const;

    Q_INVOKABLE void refreshRuntime();
    Q_INVOKABLE bool saveGhostBraveApiKey(const QString &apiKey);
    Q_INVOKABLE bool validateAndSaveGhostBraveApiKey(const QString &apiKey);
    Q_INVOKABLE bool clearGhostBraveApiKey();
    Q_INVOKABLE bool saveHeartPassWalletAddress(const QString &walletAddress);
    Q_INVOKABLE bool clearHeartPassWalletAddress();
    Q_INVOKABLE bool verifyHeartPassOwnership();
    Q_INVOKABLE void openUrl(const QString &url);

signals:
    void currentScreenChanged();
    void runtimeStateChanged();

private:
    bool generateRuntimeSnapshot();
    void loadRuntimeSnapshot();

    QString m_currentScreen;
    QString m_sessionLabel;
    QString m_systemLabel;
    QString m_walletLabel;
    QString m_agentStatus;
    QString m_runtimeStatus;
    QString m_runtimeSource;
    QString m_hostRuntimeSummary;
    bool m_online = false;
    int m_approvalsCount = 0;
    int m_notificationsCount = 0;
    QString m_lastRuntimeRefresh;
    QString m_ghostConfigStatus;
    QString m_heartPassTitle;
    QString m_heartPassStatus;
    QString m_heartPassNetwork;
    QString m_heartPassTokenStandard;
    QString m_heartPassContract;
    QString m_heartPassTokenId;
    QString m_heartPassOpenSeaUrl;
    QString m_heartPassSummary;
    QString m_heartPassNextStep;
    QString m_heartPassWalletAddress;
    QString m_heartPassOwnerAddress;
    QString m_heartPassVerificationStatus;
    QString m_heartPassLastCheckedAt;
    QString m_heartPassConfigPath;
    QStringList m_heartPassCapabilityLines;
    AppRegistryModel m_appRegistryModel;
    ActivityFeedModel m_activityFeedModel;
    QuickActionsModel m_quickActionsModel;
    ApprovalQueueModel m_approvalQueueModel;
    GhostRuntime m_ghostRuntime;
    HomeState m_homeState;
    QTimer m_runtimeWatchTimer;
};
