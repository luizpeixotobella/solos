#include "appregistrymodel.h"

AppRegistryModel::AppRegistryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_entries({
        {QStringLiteral("Workspace"), QStringLiteral("Core environment"), QStringLiteral("Tasks, notes, and active operational context for the system and the user.")},
        {QStringLiteral("Wallet Hub"), QStringLiteral("Ownership surface"), QStringLiteral("Balances, assets, identity, and future signature requests in one explicit place.")},
        {QStringLiteral("Notes Mesh"), QStringLiteral("Memory substrate"), QStringLiteral("Structured notes and memory surfaces that support continuity across sessions and agents.")}
    })
{
}

int AppRegistryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_entries.size();
}

QVariant AppRegistryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size()) {
        return {};
    }

    const auto &entry = m_entries.at(index.row());

    switch (role) {
    case NameRole:
        return entry.name;
    case SubtitleRole:
        return entry.subtitle;
    case DescriptionRole:
        return entry.description;
    default:
        return {};
    }
}

QHash<int, QByteArray> AppRegistryModel::roleNames() const
{
    return {
        {NameRole, "name"},
        {SubtitleRole, "subtitle"},
        {DescriptionRole, "description"}
    };
}
