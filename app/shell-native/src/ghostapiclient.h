#pragma once
#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QPointer>
#include <QNetworkReply>
#include <QString>
#include <QUrl>

// Text-only authenticated self-hosted client; never executes model output.
class GhostApiClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(bool configured READ configured NOTIFY changed)
    Q_PROPERTY(bool busy READ busy NOTIFY changed)
    Q_PROPERTY(QString status READ status NOTIFY changed)
    Q_PROPERTY(QString transcript READ transcript NOTIFY changed)
    Q_PROPERTY(QString endpoint READ endpoint NOTIFY changed)
    Q_PROPERTY(QString model READ model NOTIFY changed)
public:
    explicit GhostApiClient(QObject *parent = nullptr, int requestTimeoutMs = 180000);
    bool configured() const { return m_configured; }
    bool busy() const { return m_busy; }
    QString status() const { return m_status; }
    QString transcript() const { return m_transcript; }
    QString endpoint() const { return m_base.toString(); }
    QString model() const { return m_model; }
    // Blank key retains the current key only for the identical endpoint.
    Q_INVOKABLE bool configure(const QString &endpoint, const QString &key, const QString &model);
    Q_INVOKABLE bool resetConfiguration();
    Q_INVOKABLE void send(const QString &text);
    Q_INVOKABLE void clear();
signals:
    void changed();
private:
    QString configPath() const;
    bool persist(const QJsonObject &object);
    void cancel();
    void activate(const QUrl &base, const QByteArray &key, const QString &model);
    QNetworkAccessManager m_network;
    QUrl m_base;
    QByteArray m_key;
    QString m_model = "ghost-ji";
    QJsonArray m_messages;
    bool m_configured = false;
    bool m_busy = false;
    QString m_status = "Unconfigured · set up your self-hosted Ghost endpoint, key and model below";
    QString m_transcript;
    quint64 m_generation = 0;
    QPointer<QNetworkReply> m_active;
    int m_requestTimeoutMs;
};
