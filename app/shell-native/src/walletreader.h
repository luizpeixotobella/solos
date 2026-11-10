#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QDateTime>
#include <QJsonArray>
#include <QTimer>
#include <functional>

// Public address observer only: no signing, key custody, tokens or entitlement.
class WalletReader : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString address READ address NOTIFY changed)
    Q_PROPERTY(QString endpoint READ endpoint NOTIFY changed)
    Q_PROPERTY(QString status READ status NOTIFY changed)
    Q_PROPERTY(QString balance READ balance NOTIFY changed)
    Q_PROPERTY(QString checkedAt READ checkedAt NOTIFY changed)
    Q_PROPERTY(bool busy READ busy NOTIFY changed)
public:
    explicit WalletReader(QObject *parent = nullptr,
                          std::function<QDateTime()> clock = [] { return QDateTime::currentDateTimeUtc(); });
    QString address() const { return m_address; }
    QString endpoint() const { return m_endpoint; }
    QString status() const { return m_status; }
    QString balance() const { return m_balance.isEmpty() ? "Unavailable" : m_balance; }
    QString checkedAt() const { return m_checked.isValid() ? m_checked.toString(Qt::ISODate) : "Never"; }
    bool busy() const { return m_busy; }
    Q_INVOKABLE bool configure(const QString &address, const QString &endpoint);
    Q_INVOKABLE bool clear();
    Q_INVOKABLE void refresh();
signals:
    void changed();
private:
    QString configPath() const;
    void rpc(const QString &method, const QJsonArray &params, int id,
             quint64 generation, std::function<void(QString)> done);
    void fail();
    QString m_address, m_endpoint, m_balance;
    QString m_status = "Unconfigured · add a public Polygon address and RPC endpoint";
    QDateTime m_checked;
    bool m_busy = false;
    quint64 m_generation = 0;
    QNetworkAccessManager m_network;
    QTimer m_staleTimer;
    std::function<QDateTime()> m_clock;
};
