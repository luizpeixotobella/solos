#pragma once

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct AppRegistryEntry {
    QString name;
    QString subtitle;
    QString description;
};

class AppRegistryModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        NameRole = Qt::UserRole + 1,
        SubtitleRole,
        DescriptionRole
    };

    explicit AppRegistryModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<AppRegistryEntry> m_entries;
};
