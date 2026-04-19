#pragma once

#include <QObject>
#include <QString>

class GhostRuntime : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString presenceLabel READ presenceLabel NOTIFY stateChanged)
    Q_PROPERTY(QString modeLabel READ modeLabel NOTIFY stateChanged)
    Q_PROPERTY(QString thesisLabel READ thesisLabel NOTIFY stateChanged)

public:
    explicit GhostRuntime(QObject *parent = nullptr);

    QString presenceLabel() const;
    QString modeLabel() const;
    QString thesisLabel() const;

    void setLabels(const QString &presence, const QString &mode, const QString &thesis);

signals:
    void stateChanged();

private:
    QString m_presenceLabel;
    QString m_modeLabel;
    QString m_thesisLabel;
};
