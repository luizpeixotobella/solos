#include "activityfeedmodel.h"

ActivityFeedModel::ActivityFeedModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_entries({
        {QStringLiteral("Workspace resume requested"), QStringLiteral("The shell is prepared to surface a resumed workspace session when the runtime bridge is connected."), QStringLiteral("queued")},
        {QStringLiteral("Approval channel visible"), QStringLiteral("Sensitive actions should eventually appear here with scope, actor, and consequence before execution."), QStringLiteral("ready")},
        {QStringLiteral("Ghost heartbeat nominal"), QStringLiteral("Agent presence is active. Next step is replacing this mock feed with real task and event data."), QStringLiteral("active")}
    })
{
}

int ActivityFeedModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_entries.size();
}

QVariant ActivityFeedModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size()) {
        return {};
    }

    const auto &entry = m_entries.at(index.row());

    switch (role) {
    case TitleRole:
        return entry.title;
    case DetailRole:
        return entry.detail;
    case StatusRole:
        return entry.status;
    default:
        return {};
    }
}

QHash<int, QByteArray> ActivityFeedModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {DetailRole, "detail"},
        {StatusRole, "status"}
    };
}
