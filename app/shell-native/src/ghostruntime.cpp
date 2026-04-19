#include "ghostruntime.h"

GhostRuntime::GhostRuntime(QObject *parent)
    : QObject(parent)
    , m_presenceLabel(QStringLiteral("Ghost present in shell"))
    , m_modeLabel(QStringLiteral("Observing · approval-aware · not yet system-bound"))
    , m_thesisLabel(QStringLiteral("Ghost should become a native orchestration layer, not a floating chat pane."))
{
}

QString GhostRuntime::presenceLabel() const
{
    return m_presenceLabel;
}

QString GhostRuntime::modeLabel() const
{
    return m_modeLabel;
}

QString GhostRuntime::thesisLabel() const
{
    return m_thesisLabel;
}

void GhostRuntime::setLabels(const QString &presence, const QString &mode, const QString &thesis)
{
    m_presenceLabel = presence;
    m_modeLabel = mode;
    m_thesisLabel = thesis;
    emit stateChanged();
}
