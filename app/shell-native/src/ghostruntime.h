#pragma once

#include <QObject>
#include <QString>

class GhostRuntime : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString presenceLabel READ presenceLabel CONSTANT)
    Q_PROPERTY(QString modeLabel READ modeLabel CONSTANT)
    Q_PROPERTY(QString thesisLabel READ thesisLabel CONSTANT)

public:
    explicit GhostRuntime(QObject *parent = nullptr);

    QString presenceLabel() const;
    QString modeLabel() const;
    QString thesisLabel() const;
};
