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

    Q_INVOKABLE void refreshRuntime();
    Q_INVOKABLE bool saveGhostBraveApiKey(const QString &apiKey);
    Q_INVOKABLE bool validateAndSaveGhostBraveApiKey(const QString &apiKey);
    Q_INVOKABLE bool clearGhostBraveApiKey();
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
    AppRegistryModel m_appRegistryModel;
    ActivityFeedModel m_activityFeedModel;
    QuickActionsModel m_quickActionsModel;
    ApprovalQueueModel m_approvalQueueModel;
    GhostRuntime m_ghostRuntime;
    HomeState m_homeState;
    QTimer m_runtimeWatchTimer;
};
