#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct ApprovalQueueEntry {
    QString id;
    QString title;
    QString description;
    QString requestedBy;
    QString capability;
    QString scope;
    QString risk;
    QString status;
    QString createdAt;
};

class ApprovalQueueModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        DescriptionRole,
        RequestedByRole,
        CapabilityRole,
        ScopeRole,
        RiskRole,
        StatusRole,
        CreatedAtRole
    };

    explicit ApprovalQueueModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setEntries(const QVector<ApprovalQueueEntry> &entries);

private:
    QVector<ApprovalQueueEntry> m_entries;
};
