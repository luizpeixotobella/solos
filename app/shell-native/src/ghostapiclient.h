#pragma once
#include <QObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QString>
#include <QUrl>

// Text-only client. Host actions continue through the existing SolOS Daemon.
class GhostApiClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool configured READ configured CONSTANT)
    Q_PROPERTY(bool busy READ busy NOTIFY changed)
    Q_PROPERTY(QString status READ status NOTIFY changed)
    Q_PROPERTY(QString transcript READ transcript NOTIFY changed)
public:
    explicit GhostApiClient(QObject *parent = nullptr);
    bool configured() const { return m_configured; }
    bool busy() const { return m_busy; }
    QString status() const { return m_status; }
    QString transcript() const { return m_transcript; }
    Q_INVOKABLE void send(const QString &text);
    Q_INVOKABLE void clear();
signals:
    void changed();
private:
    QNetworkAccessManager m_network;
    QUrl m_endpoint;
    QByteArray m_key;
    QJsonArray m_messages;
    bool m_configured = false;
    bool m_busy = false;
    QString m_status;
    QString m_transcript;
};
