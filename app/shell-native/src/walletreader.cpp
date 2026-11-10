#include "walletreader.h"
#include "chainrpc.h"
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QStandardPaths>
#include <QJsonArray>
#include <QNetworkReply>
#include <QNetworkRequest>

QString WalletReader::configPath() const {
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/solos/wallet.json";
}
WalletReader::WalletReader(QObject *parent, std::function<QDateTime()> clock)
    : QObject(parent), m_network(this), m_clock(std::move(clock)) {
    QFile file(configPath());
    if (file.exists()) {
        if (file.open(QIODevice::ReadOnly)) {
            const auto obj = QJsonDocument::fromJson(file.read(16385)).object();
            const auto a = obj.value("address").toString();
            const auto e = obj.value("endpoint").toString();
            if (ChainRpc::address(a) && ChainRpc::endpoint(QUrl(e))) {
                m_address = a; m_endpoint = e;
                m_status = "Read-only address configured · not queried this session";
            } else m_status = "Invalid saved wallet configuration · set up again";
        } else m_status = "Wallet configuration unreadable · set up again";
    }
    m_staleTimer.setInterval(1000);
    connect(&m_staleTimer, &QTimer::timeout, this, [this] {
        if (!m_busy && m_checked.isValid() && m_checked.secsTo(m_clock()) >= 300
            && !m_balance.isEmpty()) {
            m_balance.clear(); m_status = "Stale · refresh to read the balance again"; emit changed();
        }
    });
    m_staleTimer.start();
}
bool WalletReader::configure(const QString &address, const QString &endpoint) {
    const auto a = address.trimmed(); const auto e = endpoint.trimmed();
    if (!ChainRpc::address(a) || !ChainRpc::endpoint(QUrl(e))) {
        ++m_generation; m_busy = false; m_balance.clear();
        m_status = "Invalid setup · nonzero 0x address; HTTPS RPC or localhost HTTP required";
        emit changed(); return false;
    }
    QDir dir(QFileInfo(configPath()).absolutePath());
    if (!dir.mkpath(".") || !QFile::setPermissions(dir.path(), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner)) {
        m_status = "Cannot create private wallet directory"; emit changed(); return false;
    }
    QSaveFile file(configPath());
    const auto payload = QJsonDocument(QJsonObject{{"address", a}, {"endpoint", e}}).toJson();
    if (!file.open(QIODevice::WriteOnly) || !file.setPermissions(QFile::ReadOwner | QFile::WriteOwner)
        || file.write(payload) != payload.size() || !file.commit()) {
        m_status = "Could not save wallet configuration"; emit changed(); return false;
    }
    ++m_generation; m_busy = false; m_address = a; m_endpoint = e; m_balance.clear(); m_checked = {};
    m_status = "Read-only address saved · no proof of control or entitlement";
    emit changed(); return true;
}
bool WalletReader::clear() {
    QFile file(configPath());
    if (file.exists() && !file.remove()) { m_status = "Could not clear saved address"; emit changed(); return false; }
    ++m_generation; m_busy = false; m_address.clear(); m_endpoint.clear(); m_balance.clear(); m_checked = {};
    m_status = "Unconfigured · no account or balance available"; emit changed(); return true;
}
void WalletReader::fail() {
    m_busy = false; m_balance.clear(); m_status = "RPC failed or wrong chain/response · balance unavailable";
    emit changed();
}
void WalletReader::rpc(const QString &method, const QJsonArray &params, int id,
                       quint64 generation, std::function<void(QString)> done) {
    QNetworkRequest request{QUrl(m_endpoint)};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    request.setTransferTimeout(5000);
    auto reply = m_network.post(request, QJsonDocument(QJsonObject{
        {"jsonrpc", "2.0"}, {"id", id}, {"method", method}, {"params", params}}).toJson());
    auto timer = new QTimer(reply); timer->setSingleShot(true);
    connect(timer, &QTimer::timeout, reply, &QNetworkReply::abort); timer->start(5000);
    connect(reply, &QNetworkReply::readyRead, reply, [reply] {
        if (reply->bytesAvailable() > 65536) reply->abort();
    });
    connect(reply, &QNetworkReply::finished, this, [this, reply, timer, generation, id, done] {
        timer->stop(); const auto bytes = reply->readAll();
        const bool ok = reply->error() == QNetworkReply::NoError
            && reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200 && bytes.size() <= 65536;
        reply->deleteLater();
        if (generation != m_generation) return;
        if (!ok) { fail(); return; }
        const auto result = ChainRpc::result(bytes, id);
        if (result.isEmpty()) { fail(); return; }
        done(result);
    });
}
void WalletReader::refresh() {
    if (m_busy || m_address.isEmpty()) return;
    m_busy = true; m_balance.clear(); m_status = "Checking Polygon chain ID…"; emit changed();
    const auto generation = ++m_generation;
    rpc("eth_chainId", {}, 1, generation, [this, generation](QString chain) {
        if (chain.toLower() != "0x89") { fail(); return; }
        rpc("eth_getBalance", {m_address, "latest"}, 2, generation, [this](QString balance) {
            if (!ChainRpc::quantity(balance)) { fail(); return; }
            m_balance = ChainRpc::nativeAmount(balance); m_checked = m_clock(); m_busy = false;
            m_status = "Read-only RPC observation · not proof of control; expires after 5 minutes";
            emit changed();
        });
    });
}
