#include "ghostruntime.h"

GhostRuntime::GhostRuntime(QObject *parent)
    : QObject(parent)
{
}

QString GhostRuntime::presenceLabel() const
{
    return QStringLiteral("Ghost present in shell");
}

QString GhostRuntime::modeLabel() const
{
    return QStringLiteral("Observing · approval-aware · not yet system-bound");
}

QString GhostRuntime::thesisLabel() const
{
    return QStringLiteral("Ghost should become a native orchestration layer, not a floating chat pane.");
}
