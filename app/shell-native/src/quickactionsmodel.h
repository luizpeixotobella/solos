#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct QuickActionEntry {
    QString title;
    QString subtitle;
    QString description;
};

class QuickActionsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        SubtitleRole,
        DescriptionRole
    };

    explicit QuickActionsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void setEntries(const QVector<QuickActionEntry> &entries);

private:
    QVector<QuickActionEntry> m_entries;
};
