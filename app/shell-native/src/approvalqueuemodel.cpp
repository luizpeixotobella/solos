#include "approvalqueuemodel.h"

ApprovalQueueModel::ApprovalQueueModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_entries({
        {QStringLiteral("Resume workspace session"), QStringLiteral("System shell"), QStringLiteral("low")},
        {QStringLiteral("Connect signing surface"), QStringLiteral("Wallet"), QStringLiteral("medium")}
    })
{
}

int ApprovalQueueModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return m_entries.size();
}

QVariant ApprovalQueueModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_entries.size()) {
        return {};
    }

    const auto &entry = m_entries.at(index.row());

    switch (role) {
    case TitleRole:
        return entry.title;
    case ScopeRole:
        return entry.scope;
    case RiskRole:
        return entry.risk;
    default:
        return {};
    }
}

QHash<int, QByteArray> ApprovalQueueModel::roleNames() const
{
    return {
        {TitleRole, "title"},
        {ScopeRole, "scope"},
        {RiskRole, "risk"}
    };
}
