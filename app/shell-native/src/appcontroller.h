#pragma once

#include <QObject>
#include <QStringList>

#include "activityfeedmodel.h"
#include "appregistrymodel.h"
#include "approvalqueuemodel.h"
#include "ghostruntime.h"
#include "quickactionsmodel.h"

class AppController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString currentScreen READ currentScreen WRITE setCurrentScreen NOTIFY currentScreenChanged)
    Q_PROPERTY(QString sessionLabel READ sessionLabel CONSTANT)
    Q_PROPERTY(QString systemLabel READ systemLabel CONSTANT)
    Q_PROPERTY(QString walletLabel READ walletLabel CONSTANT)
    Q_PROPERTY(QString agentStatus READ agentStatus CONSTANT)
    Q_PROPERTY(QStringList appNames READ appNames CONSTANT)
    Q_PROPERTY(AppRegistryModel* appRegistryModel READ appRegistryModel CONSTANT)
    Q_PROPERTY(ActivityFeedModel* activityFeedModel READ activityFeedModel CONSTANT)
    Q_PROPERTY(QuickActionsModel* quickActionsModel READ quickActionsModel CONSTANT)
    Q_PROPERTY(ApprovalQueueModel* approvalQueueModel READ approvalQueueModel CONSTANT)
    Q_PROPERTY(GhostRuntime* ghostRuntime READ ghostRuntime CONSTANT)

public:
    explicit AppController(QObject *parent = nullptr);

    QString currentScreen() const;
    void setCurrentScreen(const QString &screen);

    QString sessionLabel() const;
    QString systemLabel() const;
    QString walletLabel() const;
    QString agentStatus() const;
    QStringList appNames() const;
    AppRegistryModel *appRegistryModel();
    ActivityFeedModel *activityFeedModel();
    QuickActionsModel *quickActionsModel();
    ApprovalQueueModel *approvalQueueModel();
    GhostRuntime *ghostRuntime();

signals:
    void currentScreenChanged();

private:
    QString m_currentScreen;
    AppRegistryModel m_appRegistryModel;
    ActivityFeedModel m_activityFeedModel;
    QuickActionsModel m_quickActionsModel;
    ApprovalQueueModel m_approvalQueueModel;
    GhostRuntime m_ghostRuntime;
};
