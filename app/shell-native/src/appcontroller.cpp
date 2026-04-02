#include "appcontroller.h"

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_currentScreen(QStringLiteral("Home"))
    , m_appRegistryModel(this)
    , m_activityFeedModel(this)
    , m_quickActionsModel(this)
    , m_approvalQueueModel(this)
    , m_ghostRuntime(this)
{
}

QString AppController::currentScreen() const
{
    return m_currentScreen;
}

void AppController::setCurrentScreen(const QString &screen)
{
    if (m_currentScreen == screen) {
        return;
    }

    m_currentScreen = screen;
    emit currentScreenChanged();
}

QString AppController::sessionLabel() const
{
    return QStringLiteral("Luiz · SolOS Environment Active");
}

QString AppController::systemLabel() const
{
    return QStringLiteral("Online · v0.1-foundation · Synced");
}

QString AppController::walletLabel() const
{
    return QStringLiteral("Solana · 9xLu...Ghost · 12.84 SOL");
}

QString AppController::agentStatus() const
{
    return QStringLiteral("Ghost active · awaiting approval");
}

QStringList AppController::appNames() const
{
    return {
        QStringLiteral("Workspace"),
        QStringLiteral("Wallet Hub"),
        QStringLiteral("Notes Mesh")
    };
}

AppRegistryModel *AppController::appRegistryModel()
{
    return &m_appRegistryModel;
}

ActivityFeedModel *AppController::activityFeedModel()
{
    return &m_activityFeedModel;
}

QuickActionsModel *AppController::quickActionsModel()
{
    return &m_quickActionsModel;
}

ApprovalQueueModel *AppController::approvalQueueModel()
{
    return &m_approvalQueueModel;
}

GhostRuntime *AppController::ghostRuntime()
{
    return &m_ghostRuntime;
}
