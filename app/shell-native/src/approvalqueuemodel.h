#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct ApprovalQueueEntry {
    QString title;
    QString scope;
    QString risk;
};

class ApprovalQueueModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        ScopeRole,
        RiskRole
    };

    explicit ApprovalQueueModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setEntries(const QVector<ApprovalQueueEntry> &entries);

private:
    QVector<ApprovalQueueEntry> m_entries;
};
