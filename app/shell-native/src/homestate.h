#pragma once

#include <QObject>
#include <QString>

class HomeState : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString summaryTitle READ summaryTitle CONSTANT)
    Q_PROPERTY(QString summarySubtitle READ summarySubtitle CONSTANT)
    Q_PROPERTY(QString summaryBody READ summaryBody CONSTANT)
    Q_PROPERTY(QString nextActionTitle READ nextActionTitle CONSTANT)
    Q_PROPERTY(QString nextActionSubtitle READ nextActionSubtitle CONSTANT)
    Q_PROPERTY(QString nextActionBody READ nextActionBody CONSTANT)

public:
    explicit HomeState(QObject *parent = nullptr);

    QString summaryTitle() const;
    QString summarySubtitle() const;
    QString summaryBody() const;
    QString nextActionTitle() const;
    QString nextActionSubtitle() const;
    QString nextActionBody() const;
};
