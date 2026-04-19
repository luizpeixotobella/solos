#pragma once

#include <QObject>
#include <QString>

class HomeState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString summaryTitle READ summaryTitle NOTIFY stateChanged)
    Q_PROPERTY(QString summarySubtitle READ summarySubtitle NOTIFY stateChanged)
    Q_PROPERTY(QString summaryBody READ summaryBody NOTIFY stateChanged)
    Q_PROPERTY(QString nextActionTitle READ nextActionTitle NOTIFY stateChanged)
    Q_PROPERTY(QString nextActionSubtitle READ nextActionSubtitle NOTIFY stateChanged)
    Q_PROPERTY(QString nextActionBody READ nextActionBody NOTIFY stateChanged)

public:
    explicit HomeState(QObject *parent = nullptr);

    QString summaryTitle() const;
    QString summarySubtitle() const;
    QString summaryBody() const;
    QString nextActionTitle() const;
    QString nextActionSubtitle() const;
    QString nextActionBody() const;

    void setSummary(const QString &title, const QString &subtitle, const QString &body);
    void setNextAction(const QString &title, const QString &subtitle, const QString &body);

signals:
    void stateChanged();

private:
    QString m_summaryTitle;
    QString m_summarySubtitle;
    QString m_summaryBody;
    QString m_nextActionTitle;
    QString m_nextActionSubtitle;
    QString m_nextActionBody;
};
