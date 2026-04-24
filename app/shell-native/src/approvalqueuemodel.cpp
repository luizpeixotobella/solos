#include "approvalqueuemodel.h"

ApprovalQueueModel::ApprovalQueueModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_entries({
        {
            QStringLiteral("approval-resume-workspace"),
            QStringLiteral("Resume workspace session"),
            QStringLiteral("Restore the current SolOS workspace context through a bounded shell action."),
            QStringLiteral("ghost-console"),
            QStringLiteral("workspace.session.resume"),
            QStringLiteral("System shell"),
            QStringLiteral("low"),
            QStringLiteral("pending"),
            QStringLiteral("boot")
        },
        {
            QStringLiteral("approval-connect-wallet"),
            QStringLiteral("Connect signing surface"),
            QStringLiteral("Prepare a visible signing surface without hiding wallet control inside the agent."),
            QStringLiteral("wallet-hub"),
            QStringLiteral("wallet.sign.request"),
            QStringLiteral("Wallet"),
            QStringLiteral("medium"),
            QStringLiteral("pending"),
            QStringLiteral("boot")
        }
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
    case IdRole:
        return entry.id;
    case TitleRole:
        return entry.title;
    case DescriptionRole:
        return entry.description;
    case RequestedByRole:
        return entry.requestedBy;
    case CapabilityRole:
        return entry.capability;
    case ScopeRole:
        return entry.scope;
    case RiskRole:
        return entry.risk;
    case StatusRole:
        return entry.status;
    case CreatedAtRole:
        return entry.createdAt;
    default:
        return {};
    }
}

QHash<int, QByteArray> ApprovalQueueModel::roleNames() const
{
    return {
        {IdRole, "id"},
        {TitleRole, "title"},
        {DescriptionRole, "description"},
        {RequestedByRole, "requestedBy"},
        {CapabilityRole, "capability"},
        {ScopeRole, "scope"},
        {RiskRole, "risk"},
        {StatusRole, "status"},
        {CreatedAtRole, "createdAt"}
    };
}

void ApprovalQueueModel::setEntries(const QVector<ApprovalQueueEntry> &entries)
{
    beginResetModel();
    m_entries = entries;
    endResetModel();
}
