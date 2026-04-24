#include "appcontroller.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QUrl>

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
    , m_hostRuntimeSummary(QStringLiteral("host runtime summary unavailable"))
    , m_online(false)
    , m_approvalsCount(0)
    , m_notificationsCount(0)
    , m_lastRuntimeRefresh(QStringLiteral("not yet refreshed"))
    , m_ghostConfigStatus(QStringLiteral("Ghost Brave key not configured"))
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

QString AppController::hostRuntimeSummary() const
{
    return m_hostRuntimeSummary;
}

bool AppController::online() const
{
    return m_online;
}

int AppController::approvalsCount() const
{
    return m_approvalsCount;
}

int AppController::notificationsCount() const
{
    return m_notificationsCount;
}

QString AppController::lastRuntimeRefresh() const
{
    return m_lastRuntimeRefresh;
}

QString AppController::ghostConfigStatus() const
{
    return m_ghostConfigStatus;
}

void AppController::refreshRuntime()
{
    loadRuntimeSnapshot();
}

namespace {
QString ghostConfigPath()
{
    return QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../../../config/ghost.json")));
}

bool loadGhostConfigJson(QJsonObject &root)
{
    QFile file(ghostConfigPath());
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!document.isObject()) {
        return false;
    }

    root = document.object();
    return true;
}

bool writeGhostConfigJson(const QJsonObject &root)
{
    QFile file(ghostConfigPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}
}

bool AppController::saveGhostBraveApiKey(const QString &apiKey)
{
    const QString trimmed = apiKey.trimmed();
    if (trimmed.isEmpty()) {
        m_ghostConfigStatus = QStringLiteral("Ghost Brave key is empty");
        emit runtimeStateChanged();
        return false;
    }

    QJsonObject root;
    if (!loadGhostConfigJson(root)) {
        m_ghostConfigStatus = QStringLiteral("Could not open Ghost config");
        emit runtimeStateChanged();
        return false;
    }

    QJsonObject ghost = root.value(QStringLiteral("ghost")).toObject();
    QJsonObject intelligence = ghost.value(QStringLiteral("intelligence")).toObject();
    QJsonObject webSearch = intelligence.value(QStringLiteral("webSearch")).toObject();

    webSearch.insert(QStringLiteral("apiKey"), trimmed);
    webSearch.insert(QStringLiteral("enabled"), true);
    webSearch.insert(QStringLiteral("status"), QStringLiteral("configured"));
    intelligence.insert(QStringLiteral("webSearch"), webSearch);
    ghost.insert(QStringLiteral("intelligence"), intelligence);
    root.insert(QStringLiteral("ghost"), ghost);

    if (!writeGhostConfigJson(root)) {
        m_ghostConfigStatus = QStringLiteral("Could not write Ghost config");
        emit runtimeStateChanged();
        return false;
    }

    m_ghostConfigStatus = QStringLiteral("Ghost Brave key saved in SolOS repo config");
    loadRuntimeSnapshot();
    emit runtimeStateChanged();
    return true;
}

bool AppController::validateAndSaveGhostBraveApiKey(const QString &apiKey)
{
    const QString trimmed = apiKey.trimmed();
    if (trimmed.isEmpty()) {
        m_ghostConfigStatus = QStringLiteral("Ghost Brave key is empty");
        emit runtimeStateChanged();
        return false;
    }

    const QString command = QStringLiteral(
        "curl -fsSL --max-time 12 -H 'Accept: application/json' -H 'X-Subscription-Token: %1' 'https://api.search.brave.com/res/v1/web/search?q=solos&count=1'"
    ).arg(trimmed);

    QProcess process;
    process.start(QStringLiteral("sh"), {QStringLiteral("-lc"), command});
    if (!process.waitForFinished(15000)) {
        m_ghostConfigStatus = QStringLiteral("Brave key validation timed out");
        emit runtimeStateChanged();
        return false;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        const QString error = QString::fromUtf8(process.readAllStandardError()).trimmed();
        m_ghostConfigStatus = error.isEmpty()
            ? QStringLiteral("Brave key validation failed")
            : QStringLiteral("Brave key invalid: ") + error;
        emit runtimeStateChanged();
        return false;
    }

    m_ghostConfigStatus = QStringLiteral("Brave key validated, saving into SolOS repo config");
    emit runtimeStateChanged();
    return saveGhostBraveApiKey(trimmed);
}

bool AppController::clearGhostBraveApiKey()
{
    QJsonObject root;
    if (!loadGhostConfigJson(root)) {
        m_ghostConfigStatus = QStringLiteral("Could not open Ghost config");
        emit runtimeStateChanged();
        return false;
    }

    QJsonObject ghost = root.value(QStringLiteral("ghost")).toObject();
    QJsonObject intelligence = ghost.value(QStringLiteral("intelligence")).toObject();
    QJsonObject webSearch = intelligence.value(QStringLiteral("webSearch")).toObject();

    webSearch.insert(QStringLiteral("apiKey"), QStringLiteral(""));
    webSearch.insert(QStringLiteral("enabled"), false);
    webSearch.insert(QStringLiteral("status"), QStringLiteral("needs-user-key"));
    intelligence.insert(QStringLiteral("webSearch"), webSearch);
    ghost.insert(QStringLiteral("intelligence"), intelligence);
    root.insert(QStringLiteral("ghost"), ghost);

    if (!writeGhostConfigJson(root)) {
        m_ghostConfigStatus = QStringLiteral("Could not clear Ghost config");
        emit runtimeStateChanged();
        return false;
    }

    m_ghostConfigStatus = QStringLiteral("Ghost Brave key cleared from SolOS repo config");
    loadRuntimeSnapshot();
    emit runtimeStateChanged();
    return true;
}

void AppController::openUrl(const QString &url)
{
    QDesktopServices::openUrl(QUrl(url));
}

void AppController::loadRuntimeSnapshot()
{
    const RuntimeSnapshotData snapshot = RuntimeBridge::loadSnapshot(m_runtimeSource);
    const QString now = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"));

    if (!snapshot.isValid) {
        m_runtimeStatus = QStringLiteral("Runtime intermediary snapshot missing or invalid");
        m_hostRuntimeSummary = QStringLiteral("host runtime summary unavailable");
        m_online = false;
        m_approvalsCount = 0;
        m_notificationsCount = 0;
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

    m_hostRuntimeSummary = snapshot.hostRuntimeSummary;
    m_online = snapshot.online;
    m_approvalsCount = snapshot.approvalsCount;
    m_notificationsCount = snapshot.notificationsCount;
    m_lastRuntimeRefresh = now;
    m_ghostConfigStatus = snapshot.ghostOnboardingStatus == QStringLiteral("configured")
        ? QStringLiteral("Ghost Brave key configured in SolOS repo")
        : QStringLiteral("Ghost Brave key not configured yet, user must bring their own key");

    m_homeState.setSummary(snapshot.summaryTitle, snapshot.summarySubtitle, snapshot.summaryBody);
    m_homeState.setNextAction(snapshot.nextActionTitle, snapshot.nextActionSubtitle, snapshot.nextActionBody);
    m_ghostRuntime.setLabels(snapshot.ghostPresenceLabel,
                             snapshot.ghostModeLabel,
                             snapshot.ghostThesisLabel,
                             snapshot.ghostIntelligenceSummary,
                             snapshot.ghostWebStatusLabel,
                             snapshot.ghostResearchQuery,
                             snapshot.ghostResearchSummary,
                             snapshot.ghostOnboardingTitle,
                             snapshot.ghostOnboardingBody,
                             snapshot.ghostOnboardingUrl,
                             snapshot.ghostOnboardingStatus,
                             snapshot.ghostPipelineLines,
                             snapshot.ghostCitationLines);
    m_quickActionsModel.setEntries(snapshot.quickActions);
    m_activityFeedModel.setEntries(snapshot.activityFeed);
    m_approvalQueueModel.setEntries(snapshot.approvals);
    if (!snapshot.apps.isEmpty()) {
        m_appRegistryModel.setEntries(snapshot.apps);
    }

    emit runtimeStateChanged();
}
