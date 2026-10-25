#include "ghostapiclient.h"
#include <QFile>
#include <QRegularExpression>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>

GhostApiClient::GhostApiClient(QObject *parent) : QObject(parent), m_network(this) {
    const QString base = qEnvironmentVariable("SOLOS_GHOST_API_URL", QStringLiteral("http://127.0.0.1:18791/v1"));
    m_endpoint = QUrl(base);
    const bool local = m_endpoint.host() == "127.0.0.1" || m_endpoint.host() == "localhost" || m_endpoint.host() == "::1";
    if (!m_endpoint.isValid() || m_endpoint.host().isEmpty() || !m_endpoint.userInfo().isEmpty()
        || m_endpoint.hasQuery() || m_endpoint.hasFragment()
        || (m_endpoint.scheme() != "https" && !(local && m_endpoint.scheme() == "http"))) {
        m_status = QStringLiteral("Ghost needs HTTPS or a private localhost tunnel."); return;
    }
    m_endpoint.setPath(m_endpoint.path().remove(QRegularExpression("/+$")) + "/chat/completions");
    const QString keyPath = qEnvironmentVariable("SOLOS_GHOST_API_KEY_FILE");
    QFile keyFile(keyPath);
    if (keyPath.isEmpty() || !keyFile.open(QIODevice::ReadOnly)) {
        m_status = QStringLiteral("Set SOLOS_GHOST_API_KEY_FILE to your protected Ghost key file, then restart SolOS."); return;
    }
    if (keyFile.permissions() & (QFile::ReadGroup | QFile::WriteGroup | QFile::ReadOther | QFile::WriteOther)) {
        m_status = QStringLiteral("Ghost key file must be private (chmod 600)."); return;
    }
    m_key = keyFile.read(4097).trimmed();
    if (m_key.size() < 32 || m_key.size() > 4096 || m_key.contains('\n') || m_key.contains('\r')) {
        m_status = QStringLiteral("Invalid Ghost key file."); return;
    }
    m_configured = true;
    m_status = QStringLiteral("Ghost API configured · conversation stays in this session.");
}

void GhostApiClient::clear() {
    if (m_busy) return;
    m_messages = {}; m_transcript.clear();
    m_status = m_configured ? QStringLiteral("Conversation cleared from this session.") : m_status;
    emit changed();
}

void GhostApiClient::send(const QString &text) {
    if (!m_configured || m_busy || text.trimmed().isEmpty()) return;
    if (text.size() > 4000) { m_status = QStringLiteral("Please use at most 4000 characters per message."); emit changed(); return; }
    QJsonArray pending = m_messages;
    pending.append(QJsonObject{{"role", "user"}, {"content", text}});
    QNetworkRequest request(m_endpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + m_key);
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::ManualRedirectPolicy);
    request.setTransferTimeout(180000);
    m_busy = true; m_status = QStringLiteral("Ghost is thinking…"); emit changed();
    auto reply = m_network.post(request, QJsonDocument(QJsonObject{{"model", "ghost-ji"}, {"stream", false}, {"messages", pending}}).toJson(QJsonDocument::Compact));
    auto deadline = new QTimer(reply); deadline->setSingleShot(true);
    connect(deadline, &QTimer::timeout, reply, &QNetworkReply::abort); deadline->start(180000);
    connect(reply, &QNetworkReply::readyRead, reply, [reply] { if (reply->bytesAvailable() > 2 * 1024 * 1024) reply->abort(); });
    connect(reply, &QNetworkReply::finished, this, [this, reply, deadline, text, pending] {
        deadline->stop(); m_busy = false;
        const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        const auto body = QJsonDocument::fromJson(reply->readAll()).object();
        const auto choices = body.value("choices").toArray();
        const QString answer = choices.isEmpty() ? QString() : choices.first().toObject().value("message").toObject().value("content").toString();
        if (reply->error() != QNetworkReply::NoError || status != 200 || answer.isEmpty()) {
            m_status = QStringLiteral("Ghost could not answer (HTTP %1). Your message was not added; you can retry.").arg(status);
        } else {
            m_messages = pending;
            m_messages.append(QJsonObject{{"role", "assistant"}, {"content", answer}});
            // Bound session memory and history together, preserving complete user/assistant pairs.
            while (m_messages.size() > 20) { m_messages.removeAt(0); m_messages.removeAt(0); }
            m_transcript.clear();
            for (const auto &value : m_messages) {
                const auto message = value.toObject();
                m_transcript += (message.value("role") == "user" ? QStringLiteral("You: ") : QStringLiteral("Ghost: ")) + message.value("content").toString() + "\n\n";
            }
            m_status = QStringLiteral("Answered · latest 10 exchanges in session memory only.");
        }
        reply->deleteLater(); emit changed();
    });
}
