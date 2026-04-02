#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct ActivityFeedEntry {
    QString title;
    QString detail;
    QString status;
};

class ActivityFeedModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        DetailRole,
        StatusRole
    };

    explicit ActivityFeedModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<ActivityFeedEntry> m_entries;
};
