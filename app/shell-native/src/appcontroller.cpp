#include "appcontroller.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QRegularExpression>
#include <QUrl>

#include "runtimebridge.h"

namespace {
QString daemonSocketPath()
{
    const QByteArray configured = qgetenv("SOLOS_DAEMON_SOCKET");
    if (!configured.isEmpty()) {
        return QString::fromLocal8Bit(configured);
    }
    const QByteArray runtimeDirectory = qgetenv("XDG_RUNTIME_DIR");
    if (!runtimeDirectory.isEmpty()) {
        return QDir(QString::fromLocal8Bit(runtimeDirectory)).filePath(QStringLiteral("solos/daemon.sock"));
    }
    return {};
}

QString runtimeSnapshotPath()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString candidate = QDir(appDir).absoluteFilePath(QStringLiteral("../src/runtime_snapshot.json"));
    return QDir::cleanPath(candidate);
}

QString runtimeCorePath()
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString candidate = QDir(appDir).absoluteFilePath(QStringLiteral("../../runtime-core"));
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
    , m_heartPassTitle(QStringLiteral("SolOS Heart Pass"))
    , m_heartPassStatus(QStringLiteral("not loaded"))
    , m_heartPassNetwork(QStringLiteral("Polygon"))
    , m_heartPassTokenStandard(QStringLiteral("ERC-1155"))
    , m_heartPassContract(QStringLiteral("0x507783149b7abb6ce23414dd0c9742eb9f4549b4"))
    , m_heartPassTokenId(QStringLiteral("1"))
    , m_heartPassOpenSeaUrl(QStringLiteral("https://opensea.io/item/polygon/0x507783149b7abb6ce23414dd0c9742eb9f4549b4/1"))
    , m_heartPassSummary(QStringLiteral("Heart Pass state has not been loaded from the runtime snapshot yet."))
    , m_heartPassNextStep(QStringLiteral("Refresh runtime to load the Heart Pass surface."))
    , m_heartPassWalletAddress(QStringLiteral("not configured"))
    , m_heartPassOwnerAddress(QStringLiteral("not checked"))
    , m_heartPassVerificationStatus(QStringLiteral("needs-wallet"))
    , m_heartPassLastCheckedAt(QStringLiteral("never"))
    , m_heartPassConfigPath(QStringLiteral("../../../config/heart_pass.json"))
    , m_heartPassQuotaTitle(QStringLiteral("Heart Pass Quota Layer"))
    , m_heartPassQuotaStatus(QStringLiteral("verification-required"))
    , m_heartPassQuotaMode(QStringLiteral("hybrid-sponsored-byok"))
    , m_heartPassQuotaPeriod(QStringLiteral("local-pilot"))
    , m_heartPassQuotaIncludedQueries(25)
    , m_heartPassQuotaUsedQueries(0)
    , m_heartPassQuotaRemainingQueries(25)
    , m_heartPassQuotaFallback(QStringLiteral("byok"))
    , m_heartPassQuotaUsageSource(QStringLiteral("waiting-for-pass-verification"))
    , m_heartPassQuotaLastSync(QStringLiteral("never"))
    , m_heartPassQuotaResetPolicy(QStringLiteral("manual until quota service exists"))
    , m_heartPassQuotaSummary(QStringLiteral("Heart Pass quota state has not been loaded from the runtime snapshot yet."))
    , m_heartPassQuotaNextStep(QStringLiteral("Verify Heart Pass ownership before using quota as holder utility."))
    , m_appRegistryModel(this)
    , m_activityFeedModel(this)
    , m_quickActionsModel(this)
    , m_approvalQueueModel(this)
    , m_ghostRuntime(this)
    , m_homeState(this)
{
    generateRuntimeSnapshot();
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
        QStringLiteral("Notes Mesh"),
        QStringLiteral("SolOS Pulso")
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

QString AppController::heartPassTitle() const
{
    return m_heartPassTitle;
}

QString AppController::heartPassStatus() const
{
    return m_heartPassStatus;
}

QString AppController::heartPassNetwork() const
{
    return m_heartPassNetwork;
}

QString AppController::heartPassTokenStandard() const
{
    return m_heartPassTokenStandard;
}

QString AppController::heartPassContract() const
{
    return m_heartPassContract;
}

QString AppController::heartPassTokenId() const
{
    return m_heartPassTokenId;
}

QString AppController::heartPassOpenSeaUrl() const
{
    return m_heartPassOpenSeaUrl;
}

QString AppController::heartPassSummary() const
{
    return m_heartPassSummary;
}

QString AppController::heartPassNextStep() const
{
    return m_heartPassNextStep;
}

QStringList AppController::heartPassCapabilityLines() const
{
    return m_heartPassCapabilityLines;
}

QString AppController::heartPassQuotaTitle() const
{
    return m_heartPassQuotaTitle;
}

QString AppController::heartPassQuotaStatus() const
{
    return m_heartPassQuotaStatus;
}

QString AppController::heartPassQuotaMode() const
{
    return m_heartPassQuotaMode;
}

QString AppController::heartPassQuotaPeriod() const
{
    return m_heartPassQuotaPeriod;
}

int AppController::heartPassQuotaIncludedQueries() const
{
    return m_heartPassQuotaIncludedQueries;
}

int AppController::heartPassQuotaUsedQueries() const
{
    return m_heartPassQuotaUsedQueries;
}

int AppController::heartPassQuotaRemainingQueries() const
{
    return m_heartPassQuotaRemainingQueries;
}

QString AppController::heartPassQuotaFallback() const
{
    return m_heartPassQuotaFallback;
}

QString AppController::heartPassQuotaUsageSource() const
{
    return m_heartPassQuotaUsageSource;
}

QString AppController::heartPassQuotaLastSync() const
{
    return m_heartPassQuotaLastSync;
}

QString AppController::heartPassQuotaResetPolicy() const
{
    return m_heartPassQuotaResetPolicy;
}

QString AppController::heartPassQuotaSummary() const
{
    return m_heartPassQuotaSummary;
}

QString AppController::heartPassQuotaNextStep() const
{
    return m_heartPassQuotaNextStep;
}

QString AppController::heartPassWalletAddress() const
{
    return m_heartPassWalletAddress;
}

QString AppController::heartPassOwnerAddress() const
{
    return m_heartPassOwnerAddress;
}

QString AppController::heartPassVerificationStatus() const
{
    return m_heartPassVerificationStatus;
}

QString AppController::heartPassLastCheckedAt() const
{
    return m_heartPassLastCheckedAt;
}

QString AppController::heartPassConfigPath() const
{
    return m_heartPassConfigPath;
}

void AppController::refreshRuntime()
{
    generateRuntimeSnapshot();
    loadRuntimeSnapshot();
}

namespace {
QString ghostConfigPath()
{
    return QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../../../config/ghost.local.json")));
}

QString ghostTemplateConfigPath()
{
    return QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../../../config/ghost.json")));
}

QString heartPassConfigPath()
{
    return QDir::cleanPath(QDir(QCoreApplication::applicationDirPath()).absoluteFilePath(QStringLiteral("../../../config/heart_pass.json")));
}

bool loadGhostConfigJson(QJsonObject &root)
{
    QFile file(ghostConfigPath());
    if (!file.open(QIODevice::ReadOnly)) {
        file.setFileName(ghostTemplateConfigPath());
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }
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
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    file.close();
    return true;
}

QJsonObject defaultHeartPassConfig()
{
    QJsonObject root;
    root.insert(QStringLiteral("schema"), QStringLiteral("solos.heart_pass.v1"));
    root.insert(QStringLiteral("title"), QStringLiteral("SolOS Heart Pass"));
    root.insert(QStringLiteral("network"), QStringLiteral("Polygon"));
    root.insert(QStringLiteral("tokenStandard"), QStringLiteral("ERC-1155"));
    root.insert(QStringLiteral("contract"), QStringLiteral("0x507783149b7abb6ce23414dd0c9742eb9f4549b4"));
    root.insert(QStringLiteral("tokenId"), QStringLiteral("1"));
    root.insert(QStringLiteral("openSeaUrl"), QStringLiteral("https://opensea.io/item/polygon/0x507783149b7abb6ce23414dd0c9742eb9f4549b4/1"));
    root.insert(QStringLiteral("walletAddress"), QStringLiteral(""));
    root.insert(QStringLiteral("ownerAddress"), QStringLiteral(""));
    root.insert(QStringLiteral("verificationStatus"), QStringLiteral("needs-wallet"));
    root.insert(QStringLiteral("lastCheckedAt"), QStringLiteral("never"));
    root.insert(QStringLiteral("notes"), QStringLiteral("Local SolOS Heart Pass state. Wallet capture, Polygon verification, Ghost gating, and the planned quota contract stay visible before any sponsored backend is introduced."));

    QJsonObject quotaLayer;
    quotaLayer.insert(QStringLiteral("status"), QStringLiteral("planned"));
    quotaLayer.insert(QStringLiteral("mode"), QStringLiteral("hybrid-sponsored-byok"));
    quotaLayer.insert(QStringLiteral("period"), QStringLiteral("local-pilot"));
    quotaLayer.insert(QStringLiteral("includedQueries"), 25);
    quotaLayer.insert(QStringLiteral("usedQueries"), 0);
    quotaLayer.insert(QStringLiteral("remainingQueries"), 25);
    quotaLayer.insert(QStringLiteral("fallback"), QStringLiteral("byok"));
    quotaLayer.insert(QStringLiteral("usageSource"), QStringLiteral("not-active"));
    quotaLayer.insert(QStringLiteral("lastSync"), QStringLiteral("never"));
    quotaLayer.insert(QStringLiteral("resetPolicy"), QStringLiteral("manual until quota service exists"));
    quotaLayer.insert(QStringLiteral("notes"), QStringLiteral("Local placeholder for the Heart Pass Quota Layer. No sponsored provider key is used until a server-side quota service exists."));
    root.insert(QStringLiteral("quotaLayer"), quotaLayer);
    return root;
}

QJsonObject loadHeartPassConfigJson()
{
    QFile file(heartPassConfigPath());
    if (!file.open(QIODevice::ReadOnly)) {
        return defaultHeartPassConfig();
    }

    const QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!document.isObject()) {
        return defaultHeartPassConfig();
    }

    QJsonObject root = defaultHeartPassConfig();
    const QJsonObject loaded = document.object();
    for (auto it = loaded.begin(); it != loaded.end(); ++it) {
        root.insert(it.key(), it.value());
    }
    return root;
}

bool writeHeartPassConfigJson(const QJsonObject &root)
{
    QFileInfo info(heartPassConfigPath());
    QDir().mkpath(info.absolutePath());

    QFile file(heartPassConfigPath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    file.close();
    return true;
}

bool isHeartPassVerified()
{
    const QJsonObject root = loadHeartPassConfigJson();
    return root.value(QStringLiteral("verificationStatus")).toString() == QStringLiteral("verified-holder");
}

QString erc1155BalanceOfCallData(const QString &walletAddress, const QString &tokenId)
{
    bool ok = false;
    const quint64 token = tokenId.toULongLong(&ok, 10);
    const QString tokenHex = ok ? QString::number(token, 16) : QStringLiteral("1");
    const QString addressWord = walletAddress.toLower().remove(QStringLiteral("0x")).rightJustified(64, QLatin1Char('0'));
    return QStringLiteral("0x00fdd58e%1%2").arg(addressWord, tokenHex.rightJustified(64, QLatin1Char('0')));
}

quint64 extractUintFromEthCall(const QByteArray &payload, bool &ok)
{
    ok = false;
    const QJsonDocument document = QJsonDocument::fromJson(payload);
    if (!document.isObject()) {
        return 0;
    }

    const QString result = document.object().value(QStringLiteral("result")).toString();
    if (!result.startsWith(QStringLiteral("0x")) || result.length() < 3) {
        return 0;
    }

    const quint64 value = result.mid(2).toULongLong(&ok, 16);
    return value;
}
}

bool AppController::generateRuntimeSnapshot()
{
    QProcess process;
    process.setWorkingDirectory(runtimeCorePath());
    process.start(QStringLiteral("cargo"), {QStringLiteral("run")});
    if (!process.waitForFinished(30000)) {
        m_runtimeStatus = QStringLiteral("Runtime snapshot generation timed out");
        return false;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        const QString error = QString::fromUtf8(process.readAllStandardError()).trimmed();
        m_runtimeStatus = error.isEmpty()
            ? QStringLiteral("Runtime snapshot generation failed")
            : QStringLiteral("Runtime snapshot generation failed: ") + error;
        return false;
    }

    QFile snapshotFile(runtimeSnapshotPath());
    if (!snapshotFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        m_runtimeStatus = QStringLiteral("Could not write runtime snapshot file");
        return false;
    }

    snapshotFile.write(process.readAllStandardOutput());
    snapshotFile.close();
    return true;
}

bool AppController::saveGhostBraveApiKey(const QString &apiKey)
{
    if (!isHeartPassVerified()) {
        m_ghostConfigStatus = QStringLiteral("Heart Pass verification required before saving a Brave key");
        emit runtimeStateChanged();
        return false;
    }

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
    generateRuntimeSnapshot();
    loadRuntimeSnapshot();
    emit runtimeStateChanged();
    return true;
}

bool AppController::validateAndSaveGhostBraveApiKey(const QString &apiKey)
{
    if (!isHeartPassVerified()) {
        m_ghostConfigStatus = QStringLiteral("Heart Pass verification required before Brave key validation");
        emit runtimeStateChanged();
        return false;
    }

    const QString trimmed = apiKey.trimmed();
    const QRegularExpression keyPattern(QStringLiteral("^[A-Za-z0-9_-]{20,200}$"));
    if (!keyPattern.match(trimmed).hasMatch()) {
        m_ghostConfigStatus = QStringLiteral("Ghost Brave key format is invalid");
        emit runtimeStateChanged();
        return false;
    }

    QProcess process;
    process.start(QStringLiteral("curl"), {
        QStringLiteral("-fsSL"), QStringLiteral("--max-time"), QStringLiteral("12"),
        QStringLiteral("-H"), QStringLiteral("Accept: application/json"),
        QStringLiteral("-H"), QStringLiteral("X-Subscription-Token: %1").arg(trimmed),
        QStringLiteral("https://api.search.brave.com/res/v1/web/search?q=solos&count=1")
    });
    if (!process.waitForFinished(15000)) {
        m_ghostConfigStatus = QStringLiteral("Brave key validation timed out");
        emit runtimeStateChanged();
        return false;
    }

    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        m_ghostConfigStatus = QStringLiteral("Brave key validation failed");
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
    generateRuntimeSnapshot();
    loadRuntimeSnapshot();
    emit runtimeStateChanged();
    return true;
}

bool AppController::saveHeartPassWalletAddress(const QString &walletAddress)
{
    const QString trimmed = walletAddress.trimmed();
    const QRegularExpression polygonAddress(QStringLiteral("^0x[0-9a-fA-F]{40}$"));
    if (!polygonAddress.match(trimmed).hasMatch()) {
        m_heartPassStatus = QStringLiteral("invalid wallet address");
        emit runtimeStateChanged();
        return false;
    }

    QJsonObject root = loadHeartPassConfigJson();
    root.insert(QStringLiteral("walletAddress"), trimmed);
    root.insert(QStringLiteral("ownerAddress"), QStringLiteral(""));
    root.insert(QStringLiteral("verificationStatus"), QStringLiteral("wallet-configured-unverified"));
    root.insert(QStringLiteral("lastCheckedAt"), QStringLiteral("never"));

    if (!writeHeartPassConfigJson(root)) {
        m_heartPassStatus = QStringLiteral("could not write Heart Pass config");
        emit runtimeStateChanged();
        return false;
    }

    m_heartPassStatus = QStringLiteral("Heart Pass wallet saved locally");
    generateRuntimeSnapshot();
    loadRuntimeSnapshot();
    emit runtimeStateChanged();
    return true;
}

bool AppController::clearHeartPassWalletAddress()
{
    QJsonObject root = loadHeartPassConfigJson();
    root.insert(QStringLiteral("walletAddress"), QStringLiteral(""));
    root.insert(QStringLiteral("ownerAddress"), QStringLiteral(""));
    root.insert(QStringLiteral("verificationStatus"), QStringLiteral("needs-wallet"));
    root.insert(QStringLiteral("lastCheckedAt"), QStringLiteral("never"));

    if (!writeHeartPassConfigJson(root)) {
        m_heartPassStatus = QStringLiteral("could not clear Heart Pass config");
        emit runtimeStateChanged();
        return false;
    }

    m_heartPassStatus = QStringLiteral("Heart Pass wallet cleared locally");
    generateRuntimeSnapshot();
    loadRuntimeSnapshot();
    emit runtimeStateChanged();
    return true;
}

bool AppController::verifyHeartPassOwnership()
{
    QJsonObject root = loadHeartPassConfigJson();
    const QString walletAddress = root.value(QStringLiteral("walletAddress")).toString().trimmed().toLower();
    const QString contract = root.value(QStringLiteral("contract")).toString().trimmed().toLower();
    const QString tokenId = root.value(QStringLiteral("tokenId")).toString().trimmed();

    const QRegularExpression polygonAddress(QStringLiteral("^0x[0-9a-fA-F]{40}$"));
    if (!polygonAddress.match(walletAddress).hasMatch()) {
        m_heartPassStatus = QStringLiteral("needs valid wallet before verification");
        emit runtimeStateChanged();
        return false;
    }

    QJsonObject call;
    call.insert(QStringLiteral("to"), contract);
    call.insert(QStringLiteral("data"), erc1155BalanceOfCallData(walletAddress, tokenId));

    QJsonObject request;
    request.insert(QStringLiteral("jsonrpc"), QStringLiteral("2.0"));
    request.insert(QStringLiteral("id"), 1);
    request.insert(QStringLiteral("method"), QStringLiteral("eth_call"));
    request.insert(QStringLiteral("params"), QJsonArray{call, QStringLiteral("latest")});

    QProcess process;
    process.start(QStringLiteral("curl"), {
        QStringLiteral("-fsSL"),
        QStringLiteral("--max-time"),
        QStringLiteral("20"),
        QStringLiteral("-H"),
        QStringLiteral("Content-Type: application/json"),
        QStringLiteral("--data"),
        QString::fromUtf8(QJsonDocument(request).toJson(QJsonDocument::Compact)),
        QStringLiteral("https://polygon-bor-rpc.publicnode.com")
    });

    if (!process.waitForFinished(25000)) {
        process.kill();
        root.insert(QStringLiteral("verificationStatus"), QStringLiteral("verification-error"));
        root.insert(QStringLiteral("lastCheckedAt"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
        writeHeartPassConfigJson(root);
        m_heartPassStatus = QStringLiteral("Polygon verification timed out");
        generateRuntimeSnapshot();
        loadRuntimeSnapshot();
        emit runtimeStateChanged();
        return false;
    }

    const QString now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        root.insert(QStringLiteral("verificationStatus"), QStringLiteral("verification-error"));
        root.insert(QStringLiteral("lastCheckedAt"), now);
        writeHeartPassConfigJson(root);
        m_heartPassStatus = QStringLiteral("Polygon verification failed");
        generateRuntimeSnapshot();
        loadRuntimeSnapshot();
        emit runtimeStateChanged();
        return false;
    }

    bool parsedBalance = false;
    const quint64 balance = extractUintFromEthCall(process.readAllStandardOutput(), parsedBalance);
    if (!parsedBalance) {
        root.insert(QStringLiteral("verificationStatus"), QStringLiteral("verification-error"));
        root.insert(QStringLiteral("lastCheckedAt"), now);
        writeHeartPassConfigJson(root);
        m_heartPassStatus = QStringLiteral("Polygon balanceOf response was unreadable");
        generateRuntimeSnapshot();
        loadRuntimeSnapshot();
        emit runtimeStateChanged();
        return false;
    }

    root.insert(QStringLiteral("ownerAddress"), QStringLiteral("ERC-1155 balance: %1").arg(balance));
    root.insert(QStringLiteral("verificationStatus"), balance > 0 ? QStringLiteral("verified-holder") : QStringLiteral("not-holder"));
    root.insert(QStringLiteral("lastCheckedAt"), now);

    if (!writeHeartPassConfigJson(root)) {
        m_heartPassStatus = QStringLiteral("could not persist Heart Pass verification");
        emit runtimeStateChanged();
        return false;
    }

    generateRuntimeSnapshot();
    loadRuntimeSnapshot();
    emit runtimeStateChanged();
    return balance > 0;
}

bool AppController::claimPulsoGhostReward(const QString &claimCode)
{
    const QString trimmedCode = claimCode.trimmed();
    const QRegularExpression claimPattern(QStringLiteral("^[0-9a-fA-F-]{36}$"));
    QJsonObject root = loadHeartPassConfigJson();
    const QString walletAddress = root.value(QStringLiteral("walletAddress")).toString().trimmed().toLower();

    if (root.value(QStringLiteral("verificationStatus")).toString() != QStringLiteral("verified-holder")) {
        m_heartPassStatus = QStringLiteral("verify Heart Pass ownership before claiming Pulso utility");
        emit runtimeStateChanged();
        return false;
    }
    if (!claimPattern.match(trimmedCode).hasMatch()) {
        m_heartPassStatus = QStringLiteral("invalid Pulso reward claim code");
        emit runtimeStateChanged();
        return false;
    }

    const QString endpoint = qEnvironmentVariable(
        "SOLOS_PULSO_REWARDS_URL",
        QStringLiteral("http://localhost:3000/api/solos/pulso/claim")
    );
    QJsonObject payload;
    payload.insert(QStringLiteral("claimCode"), trimmedCode);
    payload.insert(QStringLiteral("walletAddress"), walletAddress);

    QProcess process;
    process.start(QStringLiteral("curl"), {
        QStringLiteral("-fsSL"), QStringLiteral("--max-time"), QStringLiteral("20"),
        QStringLiteral("-H"), QStringLiteral("Content-Type: application/json"),
        QStringLiteral("--data"), QString::fromUtf8(QJsonDocument(payload).toJson(QJsonDocument::Compact)),
        endpoint
    });

    if (!process.waitForFinished(25000) || process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        process.kill();
        m_heartPassStatus = QStringLiteral("Pulso reward service unavailable");
        emit runtimeStateChanged();
        return false;
    }

    const QJsonDocument responseDocument = QJsonDocument::fromJson(process.readAllStandardOutput());
    if (!responseDocument.isObject()) {
        m_heartPassStatus = QStringLiteral("Pulso reward response was unreadable");
        emit runtimeStateChanged();
        return false;
    }

    const QJsonObject response = responseDocument.object();
    const QString redemptionId = response.value(QStringLiteral("redemptionId")).toString();
    const int queries = response.value(QStringLiteral("queries")).toInt();
    if (redemptionId.isEmpty() || queries <= 0) {
        m_heartPassStatus = QStringLiteral("Pulso reward did not contain Ghost utility");
        emit runtimeStateChanged();
        return false;
    }

    QJsonArray claimedRedemptions = root.value(QStringLiteral("claimedPulsoRedemptions")).toArray();
    for (const QJsonValue &value : claimedRedemptions) {
        if (value.toString() == redemptionId) {
            m_heartPassStatus = QStringLiteral("Pulso reward already synchronized");
            emit runtimeStateChanged();
            return true;
        }
    }

    QJsonObject quota = root.value(QStringLiteral("quotaLayer")).toObject();
    quota.insert(QStringLiteral("status"), QStringLiteral("active"));
    quota.insert(QStringLiteral("includedQueries"), quota.value(QStringLiteral("includedQueries")).toInt() + queries);
    quota.insert(QStringLiteral("remainingQueries"), quota.value(QStringLiteral("remainingQueries")).toInt() + queries);
    quota.insert(QStringLiteral("usageSource"), QStringLiteral("pulso-founder-reward"));
    quota.insert(QStringLiteral("lastSync"), QDateTime::currentDateTimeUtc().toString(Qt::ISODate));
    quota.insert(QStringLiteral("notes"), QStringLiteral("Pulso Founder reward synchronized from the CMS ledger."));
    root.insert(QStringLiteral("quotaLayer"), quota);
    claimedRedemptions.append(redemptionId);
    root.insert(QStringLiteral("claimedPulsoRedemptions"), claimedRedemptions);

    if (!writeHeartPassConfigJson(root)) {
        m_heartPassStatus = QStringLiteral("could not persist Pulso reward");
        emit runtimeStateChanged();
        return false;
    }

    m_heartPassStatus = QStringLiteral("Pulso Founder reward synchronized");
    generateRuntimeSnapshot();
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
    const QString socketPath = daemonSocketPath();
    RuntimeSnapshotData snapshot;
    bool loadedFromDaemon = false;
    if (!socketPath.isEmpty()) {
        snapshot = RuntimeBridge::loadSnapshotFromDaemon(socketPath);
        loadedFromDaemon = snapshot.isValid;
    }
    if (!snapshot.isValid) {
        snapshot = RuntimeBridge::loadSnapshot(runtimeSnapshotPath());
    }
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

    m_runtimeSource = loadedFromDaemon
        ? QStringLiteral("SolOS Daemon · ") + socketPath
        : QStringLiteral("Snapshot fallback · ") + runtimeSnapshotPath();

    m_hostRuntimeSummary = snapshot.hostRuntimeSummary;
    m_online = snapshot.online;
    m_approvalsCount = snapshot.approvalsCount;
    m_notificationsCount = snapshot.notificationsCount;
    m_lastRuntimeRefresh = now;
    m_ghostConfigStatus = snapshot.ghostOnboardingStatus == QStringLiteral("configured")
        ? QStringLiteral("Ghost Brave key configured in SolOS repo")
        : QStringLiteral("Ghost Brave key not configured yet, user must bring their own key");
    if (!snapshot.heartPassTitle.isEmpty()) {
        m_heartPassTitle = snapshot.heartPassTitle;
    }
    m_heartPassStatus = snapshot.heartPassStatus;
    m_heartPassNetwork = snapshot.heartPassNetwork;
    m_heartPassTokenStandard = snapshot.heartPassTokenStandard;
    m_heartPassContract = snapshot.heartPassContract;
    m_heartPassTokenId = snapshot.heartPassTokenId;
    m_heartPassOpenSeaUrl = snapshot.heartPassOpenSeaUrl;
    m_heartPassSummary = snapshot.heartPassSummary;
    m_heartPassNextStep = snapshot.heartPassNextStep;
    m_heartPassWalletAddress = snapshot.heartPassWalletAddress;
    m_heartPassOwnerAddress = snapshot.heartPassOwnerAddress;
    m_heartPassVerificationStatus = snapshot.heartPassVerificationStatus;
    m_heartPassLastCheckedAt = snapshot.heartPassLastCheckedAt;
    m_heartPassConfigPath = snapshot.heartPassConfigPath;
    m_heartPassCapabilityLines = snapshot.heartPassCapabilityLines;
    m_heartPassQuotaTitle = snapshot.heartPassQuotaTitle;
    m_heartPassQuotaStatus = snapshot.heartPassQuotaStatus;
    m_heartPassQuotaMode = snapshot.heartPassQuotaMode;
    m_heartPassQuotaPeriod = snapshot.heartPassQuotaPeriod;
    m_heartPassQuotaIncludedQueries = snapshot.heartPassQuotaIncludedQueries;
    m_heartPassQuotaUsedQueries = snapshot.heartPassQuotaUsedQueries;
    m_heartPassQuotaRemainingQueries = snapshot.heartPassQuotaRemainingQueries;
    m_heartPassQuotaFallback = snapshot.heartPassQuotaFallback;
    m_heartPassQuotaUsageSource = snapshot.heartPassQuotaUsageSource;
    m_heartPassQuotaLastSync = snapshot.heartPassQuotaLastSync;
    m_heartPassQuotaResetPolicy = snapshot.heartPassQuotaResetPolicy;
    m_heartPassQuotaSummary = snapshot.heartPassQuotaSummary;
    m_heartPassQuotaNextStep = snapshot.heartPassQuotaNextStep;

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
                             snapshot.ghostIntentsTitle,
                             snapshot.ghostIntentsSummary,
                             snapshot.ghostIntentLines,
                             snapshot.ghostPipelineLines,
                             snapshot.ghostCitationLines,
                             snapshot.ghostInitiationStatus,
                             snapshot.ghostInitiationSummary,
                             snapshot.ghostInitiationDatabasePath,
                             snapshot.ghostKnowledgeLines,
                             snapshot.ghostLanguageSupportStatus,
                             snapshot.ghostLanguageSupportSummary,
                             snapshot.ghostLanguageSupportLines,
                             snapshot.ghostReadinessStatus,
                             snapshot.ghostReadinessSummary,
                             snapshot.ghostReadinessLines,
                             snapshot.ghostRequestClassifierTitle,
                             snapshot.ghostRequestClassifierSummary,
                             snapshot.ghostRequestClassificationLines,
                             snapshot.ghostActionTraceSummary,
                             snapshot.ghostRouteExplanationSummary);
    m_quickActionsModel.setEntries(snapshot.quickActions);
    m_activityFeedModel.setEntries(snapshot.activityFeed);
    m_approvalQueueModel.setEntries(snapshot.approvals);
    if (!snapshot.apps.isEmpty()) {
        m_appRegistryModel.setEntries(snapshot.apps);
    }

    emit runtimeStateChanged();
}
