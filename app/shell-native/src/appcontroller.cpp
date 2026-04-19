#include "appcontroller.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>

#include "runtimebridge.h"

namespace {
QString runtimeSnapshotPath()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString candidate = QDir(appDir).absoluteFilePath(QStringLiteral("../src/runtime_snapshot.json"));
    return QDir::cleanPath(candidate);
}
}

AppController::AppController(QObject *parent)
    : QObject(parent)
    , m_currentScreen(QStringLiteral("Home"))
    , m_sessionLabel(QStringLiteral("Luiz · SolOS Environment Active"))
    , m_systemLabel(QStringLiteral("Online · v0.1-foundation · Synced"))
    , m_walletLabel(QStringLiteral("Solana · 9xLu...Ghost · 12.84 SOL"))
    , m_agentStatus(QStringLiteral("Ghost active · awaiting approval"))
    , m_runtimeStatus(QStringLiteral("Waiting for runtime intermediary snapshot"))
    , m_runtimeSource(runtimeSnapshotPath())
    , m_lastRuntimeRefresh(QStringLiteral("not yet refreshed"))
    , m_appRegistryModel(this)
    , m_activityFeedModel(this)
    , m_quickActionsModel(this)
    , m_approvalQueueModel(this)
    , m_ghostRuntime(this)
    , m_homeState(this)
{
    loadRuntimeSnapshot();

    m_runtimeWatchTimer.setInterval(2000);
    connect(&m_runtimeWatchTimer, &QTimer::timeout, this, &AppController::refreshRuntime);
    m_runtimeWatchTimer.start();
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
    return m_sessionLabel;
}

QString AppController::systemLabel() const
{
    return m_systemLabel;
}

QString AppController::walletLabel() const
{
    return m_walletLabel;
}

QString AppController::agentStatus() const
{
    return m_agentStatus;
}

HomeState *AppController::homeState()
{
    return &m_homeState;
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

QString AppController::runtimeStatus() const
{
    return m_runtimeStatus;
}

QString AppController::runtimeSource() const
{
    return m_runtimeSource;
}

QString AppController::lastRuntimeRefresh() const
{
    return m_lastRuntimeRefresh;
}

void AppController::refreshRuntime()
{
    loadRuntimeSnapshot();
}

void AppController::loadRuntimeSnapshot()
{
    const RuntimeSnapshotData snapshot = RuntimeBridge::loadSnapshot(m_runtimeSource);
    const QString now = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));

    if (!snapshot.isValid) {
        m_runtimeStatus = QStringLiteral("Runtime intermediary snapshot missing or invalid");
        m_lastRuntimeRefresh = now;
        emit runtimeStateChanged();
        return;
    }

    m_sessionLabel = snapshot.sessionLabel;
    m_systemLabel = snapshot.systemLabel;
    m_walletLabel = snapshot.walletLabel;
    m_agentStatus = snapshot.agentStatus;

    QStringList runtimeStatusParts;
    if (!snapshot.runtimeMode.isEmpty()) {
        runtimeStatusParts << snapshot.runtimeMode;
    }
    if (!snapshot.runtimeRole.isEmpty()) {
        runtimeStatusParts << snapshot.runtimeRole;
    }
    if (!snapshot.mediationStatus.isEmpty()) {
        runtimeStatusParts << snapshot.mediationStatus;
    }
    m_runtimeStatus = runtimeStatusParts.isEmpty()
        ? QStringLiteral("Live runtime intermediary snapshot loaded")
        : runtimeStatusParts.join(QStringLiteral(" · "));

    if (!snapshot.runtimeSource.isEmpty()) {
        m_runtimeSource = snapshot.runtimeSource;
    }

    m_lastRuntimeRefresh = now;

    m_homeState.setSummary(snapshot.summaryTitle, snapshot.summarySubtitle, snapshot.summaryBody);
    m_homeState.setNextAction(snapshot.nextActionTitle, snapshot.nextActionSubtitle, snapshot.nextActionBody);
    m_ghostRuntime.setLabels(snapshot.ghostPresenceLabel, snapshot.ghostModeLabel, snapshot.ghostThesisLabel);
    m_quickActionsModel.setEntries(snapshot.quickActions);
    m_activityFeedModel.setEntries(snapshot.activityFeed);
    m_approvalQueueModel.setEntries(snapshot.approvals);
    if (!snapshot.apps.isEmpty()) {
        m_appRegistryModel.setEntries(snapshot.apps);
    }

    emit runtimeStateChanged();
}
