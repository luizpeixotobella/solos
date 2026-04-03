#include "quickactionsmodel.h"

QuickActionsModel::QuickActionsModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_entries({
        {QStringLiteral("Resume Workspace"), QStringLiteral("Context continuity"), QStringLiteral("Re-enter the operational context of the current environment with recent memory and active threads available.")},
        {QStringLiteral("Ask Ghost"), QStringLiteral("Ambient operator"), QStringLiteral("Open a direct path into the agent layer for planning, retrieval, and action delegation.")},
        {QStringLiteral("Open Wallet"), QStringLiteral("Explicit ownership"), QStringLiteral("Inspect balances, assets, and future signing surfaces without leaving the shell frame.")}
    })
{
}

int QuickActionsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_entries.size();
}

QVariant QuickActionsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size()) {
        return {};
    }

    const auto &entry = m_entries.at(index.row());

    switch (role) {
    case TitleRole:
        return entry.title;
    case SubtitleRole:
        return entry.subtitle;
    case DescriptionRole:
        return entry.description;
    default:
        return {};
    }
}

QHash<int, QByteArray> QuickActionsModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {SubtitleRole, "subtitle"},
        {DescriptionRole, "description"}
    };
}
