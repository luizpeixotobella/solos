#include "appregistrymodel.h"

AppRegistryModel::AppRegistryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_entries({
        {QStringLiteral("workspace"), QStringLiteral("Workspace"), QStringLiteral("Core environment"), QStringLiteral("Tasks, notes, and active operational context for the system and the user."), QStringLiteral("available"), QStringLiteral("app.open.safe"), QStringLiteral("screen:Home")},
        {QStringLiteral("wallet-hub"), QStringLiteral("Wallet Hub"), QStringLiteral("Ownership surface"), QStringLiteral("Balances, assets, identity, and future signature requests in one explicit place."), QStringLiteral("available"), QStringLiteral("app.open.safe"), QStringLiteral("screen:Wallet")},
        {QStringLiteral("solos-pulso"), QStringLiteral("SolOS Pulso"), QStringLiteral("Controlled Alpha adapter"), QStringLiteral("Opens the consented Pulso Alpha through an exact allowlisted route."), QStringLiteral("connected-alpha"), QStringLiteral("app.open.safe"), QStringLiteral("https://luiz-bella-artes.net/solos/pulso")}
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
    case IdRole:
        return entry.id;
    case NameRole:
        return entry.name;
    case SubtitleRole:
        return entry.subtitle;
    case DescriptionRole:
        return entry.description;
    case StatusRole:
        return entry.status;
    case CapabilityRole:
        return entry.capability;
    case LaunchTargetRole:
        return entry.launchTarget;
    default:
        return {};
    }
}

QHash<int, QByteArray> AppRegistryModel::roleNames() const
{
    return {
        {IdRole, "appId"},
        {NameRole, "name"},
        {SubtitleRole, "subtitle"},
        {DescriptionRole, "description"},
        {StatusRole, "status"},
        {CapabilityRole, "capability"},
        {LaunchTargetRole, "launchTarget"}
    };
}

void AppRegistryModel::setEntries(const QVector<AppRegistryEntry> &entries)
{
    beginResetModel();
    m_entries = entries;
    endResetModel();
}
