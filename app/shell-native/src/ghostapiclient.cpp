#include "ghostapiclient.h"
#include "chainrpc.h"
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QTimer>

namespace {
QUrl normalizedBase(const QString &input) {
    QUrl url(input.trimmed(), QUrl::StrictMode);
    url.setPath(url.path().remove(QRegularExpression("/+$")));
    return url;
}
bool validKey(const QByteArray &key) {
    if (key.size() < 32 || key.size() > 4096) return false;
    for (const char c : key) if (static_cast<unsigned char>(c) < 33 || static_cast<unsigned char>(c) > 126) return false;
    return true;
}
bool validModel(const QString &model) {
    return QRegularExpression("^[A-Za-z0-9][A-Za-z0-9._:/-]{0,127}$").match(model).hasMatch();
}
bool privateFile(const QFile &file) {
    return !(file.permissions() & (QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup
        | QFile::ReadOther | QFile::WriteOther | QFile::ExeOther));
}
}
QString GhostApiClient::configPath() const {
    return QStandardPaths::writableLocation(QStandardPaths::GenericConfigLocation) + "/solos/ghost-api.json";
}
GhostApiClient::GhostApiClient(QObject *parent, int requestTimeoutMs)
    : QObject(parent), m_network(this), m_requestTimeoutMs(requestTimeoutMs) {
    QFile file(configPath());
    if (file.exists()) {
        if (QFileInfo(file).isSymLink() || !privateFile(file) || !file.open(QIODevice::ReadOnly)) {
            m_status = "Saved Ghost configuration is unreadable or not private · set up again"; return;
        }
        const auto object = QJsonDocument::fromJson(file.read(16385)).object();
        if (object.value("schema") != "solos.ghost.client.v1") {
            m_status = "Invalid saved Ghost configuration · set up again"; return;
        }
        if (object.value("disabled").toBool()) {
            m_status = "Unconfigured · saved credentials removed; environment fallback disabled"; return;
        }
        const auto base = normalizedBase(object.value("endpoint").toString());
        const auto key = object.value("key").toString().toUtf8();
        const auto model = object.value("model").toString();
        if (!ChainRpc::endpoint(base) || !validKey(key) || !validModel(model)) {
            m_status = "Invalid saved Ghost configuration · set up again"; return;
        }
        activate(base, key, model);
        m_status = "Saved Ghost setup loaded · connection and inference not yet verified"; return;
    }
    // Compatibility only: no hosted-provider default, automatic request or persistence.
    const QString baseText = qEnvironmentVariable("SOLOS_GHOST_API_URL");
    const QString keyPath = qEnvironmentVariable("SOLOS_GHOST_API_KEY_FILE");
    if (baseText.isEmpty() && keyPath.isEmpty()) return;
    const auto base = normalizedBase(baseText.isEmpty() ? "http://127.0.0.1:18791/v1" : baseText);
    QFile keyFile(keyPath);
    if (!ChainRpc::endpoint(base) || keyPath.isEmpty() || QFileInfo(keyFile).isSymLink()
        || !privateFile(keyFile) || !keyFile.open(QIODevice::ReadOnly)) {
        m_status = "Environment setup invalid · use HTTPS/loopback and a private key file, or set up below"; return;
    }
    const auto key = keyFile.read(4097).trimmed();
    const auto model = qEnvironmentVariable("SOLOS_GHOST_MODEL", "ghost-ji");
    if (!validKey(key) || !validModel(model)) { m_status = "Invalid environment key or model · set up below"; return; }
    activate(base, key, model);
    m_status = "Environment Ghost setup loaded · connection and inference not yet verified";
}
bool GhostApiClient::persist(const QJsonObject &object) {
    const QFileInfo info(configPath());
    QDir dir(info.absolutePath());
    if (QFileInfo(dir.path()).isSymLink() || info.isSymLink() || !dir.mkpath(".")
        || !QFile::setPermissions(dir.path(), QFile::ReadOwner | QFile::WriteOwner | QFile::ExeOwner)) return false;
    QSaveFile file(configPath());
    const auto bytes = QJsonDocument(object).toJson();
    return file.open(QIODevice::WriteOnly) && file.setPermissions(QFile::ReadOwner | QFile::WriteOwner)
        && file.write(bytes) == bytes.size() && file.commit();
}
void GhostApiClient::cancel() {
    ++m_generation;
    if (m_active) { m_active->abort(); m_active.clear(); }
    m_busy = false;
}
void GhostApiClient::activate(const QUrl &base, const QByteArray &key, const QString &model) {
    cancel(); m_base = base; m_key = key; m_model = model; m_configured = true;
    m_messages = {}; m_transcript.clear();
}
bool GhostApiClient::configure(const QString &endpoint, const QString &keyText, const QString &modelText) {
    const auto base = normalizedBase(endpoint); const auto model = modelText.trimmed();
    const auto key = keyText.isEmpty() && m_configured && base == m_base ? m_key : keyText.toUtf8();
    if (endpoint.size() > 2048 || !ChainRpc::endpoint(base) || !validKey(key) || !validModel(model)) {
        m_status = "Not saved · use HTTPS/loopback, a 32–4096 character printable key and a valid model ID. A changed endpoint requires a new key. Previous setup is unchanged.";
        emit changed(); return false;
    }
    if (!persist({{"schema", "solos.ghost.client.v1"}, {"endpoint", base.toString()},
                  {"key", QString::fromUtf8(key)}, {"model", model}})) {
        m_status = "Could not save private Ghost configuration · previous setup unchanged"; emit changed(); return false;
    }
    activate(base, key, model);
    m_status = "Ghost setup saved · connection and inference not yet verified"; emit changed(); return true;
}
bool GhostApiClient::resetConfiguration() {
    // Tombstone prevents an environment credential from silently re-enabling chat at restart.
    if (!persist({{"schema", "solos.ghost.client.v1"}, {"disabled", true}})) {
        m_status = "Could not remove saved Ghost setup · configuration unchanged"; emit changed(); return false;
    }
    cancel(); m_configured = false; m_key.fill(0); m_key.clear(); m_base = QUrl(); m_model = "ghost-ji";
    m_messages = {}; m_transcript.clear();
    m_status = "Unconfigured · saved credentials removed; environment fallback disabled";
    emit changed(); return true;
}
void GhostApiClient::clear() {
    cancel(); m_messages = {}; m_transcript.clear();
    if (m_configured) m_status = "Conversation cleared from this session";
    emit changed();
}
void GhostApiClient::send(const QString &text) {
    if (!m_configured) { m_status = "Unconfigured · save your Ghost setup before sending"; emit changed(); return; }
    if (m_busy || text.trimmed().isEmpty()) return;
    if (text.size() > 4000) { m_status = "Please use at most 4000 characters per message"; emit changed(); return; }
    QJsonArray pending = m_messages;
    pending.append(QJsonObject{{"role", "user"}, {"content", text}});
    QUrl endpoint = m_base; endpoint.setPath(endpoint.path() + "/chat/completions");
    QNetworkRequest request(endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + m_key);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    request.setTransferTimeout(m_requestTimeoutMs);
    const auto generation = ++m_generation;
    m_busy = true; m_status = "Ghost is thinking…"; emit changed();
    auto reply = m_network.post(request, QJsonDocument(QJsonObject{{"model", m_model}, {"stream", false}, {"messages", pending}}).toJson(QJsonDocument::Compact));
    m_active = reply;
    auto deadline = new QTimer(reply); deadline->setSingleShot(true);
    connect(deadline, &QTimer::timeout, reply, &QNetworkReply::abort); deadline->start(m_requestTimeoutMs);
    connect(reply, &QNetworkReply::readyRead, reply, [reply] { if (reply->bytesAvailable() > 2 * 1024 * 1024) reply->abort(); });
    connect(reply, &QNetworkReply::finished, this, [this, reply, deadline, generation, pending] {
        deadline->stop();
        if (generation != m_generation) { reply->deleteLater(); return; }
        m_active.clear(); m_busy = false;
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const auto bytes = reply->readAll();
        const auto body = QJsonDocument::fromJson(bytes).object();
        const auto choices = body.value("choices").toArray();
        const QString answer = choices.isEmpty() ? QString() : choices.first().toObject().value("message").toObject().value("content").toString();
        if (reply->error() != QNetworkReply::NoError || status != 200 || body.contains("error")
            || bytes.size() > 2 * 1024 * 1024 || answer.trimmed().isEmpty()) {
            // Never echo a server error body, URL, credential or user prompt into status/logs.
            m_status = QStringLiteral("Ghost could not answer (HTTP %1). Message not added; retry or review setup.").arg(status);
        } else {
            m_messages = pending;
            m_messages.append(QJsonObject{{"role", "assistant"}, {"content", answer}});
            while (m_messages.size() > 20) { m_messages.removeAt(0); m_messages.removeAt(0); }
            m_transcript.clear();
            for (const auto &value : m_messages) {
                const auto message = value.toObject();
                m_transcript += (message.value("role") == "user" ? QStringLiteral("You: ") : QStringLiteral("Ghost: ")) + message.value("content").toString() + "\n\n";
            }
            m_status = "Answered by configured endpoint · latest 10 exchanges in session only";
        }
        reply->deleteLater(); emit changed();
    });
}
