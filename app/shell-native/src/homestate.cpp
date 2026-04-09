#include "homestate.h"

HomeState::HomeState(QObject *parent)
    : QObject(parent)
{
}

QString HomeState::summaryTitle() const
{
    return QStringLiteral("Home");
}

QString HomeState::summarySubtitle() const
{
    return QStringLiteral("What matters now");
}

QString HomeState::summaryBody() const
{
    return QStringLiteral("Home should explain the environment in one glance: agent state, approval pressure, wallet context, and the next useful move.");
}

QString HomeState::nextActionTitle() const
{
    return QStringLiteral("Review pending approvals before opening more apps");
}

QString HomeState::nextActionSubtitle() const
{
    return QStringLiteral("Ghost active · awaiting approval");
}

QString HomeState::nextActionBody() const
{
    return QStringLiteral("SolOS should bias the user toward legible action. The shell should surface the most important safe next step instead of only presenting sections.");
}
