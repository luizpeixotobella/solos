#include "homestate.h"

HomeState::HomeState(QObject *parent)
    : QObject(parent)
    , m_summaryTitle(QStringLiteral("System Summary"))
    , m_summarySubtitle(QStringLiteral("Native environment overview"))
    , m_summaryBody(QStringLiteral("The shell is establishing a more legible environment summary for the user."))
    , m_nextActionTitle(QStringLiteral("Next useful move"))
    , m_nextActionSubtitle(QStringLiteral("Clarify what matters now"))
    , m_nextActionBody(QStringLiteral("SolOS should always make the next safe, meaningful action easier to understand."))
{
}

QString HomeState::summaryTitle() const
{
    return m_summaryTitle;
}

QString HomeState::summarySubtitle() const
{
    return m_summarySubtitle;
}

QString HomeState::summaryBody() const
{
    return m_summaryBody;
}

QString HomeState::nextActionTitle() const
{
    return m_nextActionTitle;
}

QString HomeState::nextActionSubtitle() const
{
    return m_nextActionSubtitle;
}

QString HomeState::nextActionBody() const
{
    return m_nextActionBody;
}

void HomeState::setSummary(const QString &title, const QString &subtitle, const QString &body)
{
    m_summaryTitle = title;
    m_summarySubtitle = subtitle;
    m_summaryBody = body;
    emit stateChanged();
}

void HomeState::setNextAction(const QString &title, const QString &subtitle, const QString &body)
{
    m_nextActionTitle = title;
    m_nextActionSubtitle = subtitle;
    m_nextActionBody = body;
    emit stateChanged();
}
