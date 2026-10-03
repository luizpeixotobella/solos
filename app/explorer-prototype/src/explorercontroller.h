#pragma once

#include <QObject>
#include <QTimer>
#include <QVariantList>

class ExplorerController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString displayName READ displayName CONSTANT)
    Q_PROPERTY(QString currentPath READ currentPath NOTIFY filesChanged)
    Q_PROPERTY(QVariantList places READ places CONSTANT)
    Q_PROPERTY(QVariantList files READ files NOTIFY filesChanged)
    Q_PROPERTY(QVariantList apps READ apps CONSTANT)
    Q_PROPERTY(QString runtimeStatus READ runtimeStatus NOTIFY runtimeStatusChanged)

public:
    explicit ExplorerController(QObject *parent = nullptr);

    QString displayName() const;
    QString currentPath() const;
    QVariantList places() const;
    QVariantList files() const;
    QVariantList apps() const;
    QString runtimeStatus() const;

    Q_INVOKABLE void openFolder(const QString &path);
    Q_INVOKABLE void openParentFolder();
    Q_INVOKABLE void openItem(const QString &path, bool isDirectory);
    Q_INVOKABLE void launchApp(const QString &desktopFile);

signals:
    void filesChanged();
    void runtimeStatusChanged();

private slots:
    void checkRuntime();

private:
    void loadFiles();
    void loadApps();
    void startRuntime();

    QString m_displayName;
    QString m_currentPath;
    QVariantList m_places;
    QVariantList m_files;
    QVariantList m_apps;
    QString m_runtimeStatus;
    QTimer m_runtimeTimer;
    int m_runtimeChecks = 0;
};
