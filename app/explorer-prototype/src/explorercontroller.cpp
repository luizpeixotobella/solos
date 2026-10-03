#include "explorercontroller.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QHash>
#include <QLocale>
#include <QLocalSocket>
#include <QProcess>
#include <QRegularExpression>
#include <QSettings>
#include <QStandardPaths>
#include <QUrl>

namespace {
QString daemonSocketPath()
{
    const QByteArray configured = qgetenv("SOLOS_DAEMON_SOCKET");
    if (!configured.isEmpty()) {
        return QString::fromLocal8Bit(configured);
    }

    const QByteArray runtimeDirectory = qgetenv("XDG_RUNTIME_DIR");
    if (runtimeDirectory.isEmpty()) {
        return {};
    }
    return QDir(QString::fromLocal8Bit(runtimeDirectory)).filePath(QStringLiteral("solos/daemon.sock"));
}

QStringList desktopEntryDirectories()
{
    QStringList directories;
    for (const QString &dataDirectory : QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation)) {
        directories.append(QDir(dataDirectory).filePath(QStringLiteral("applications")));
    }
    directories.removeDuplicates();
    return directories;
}

QStringList desktopExecArguments(const QString &execLine)
{
    QStringList arguments = QProcess::splitCommand(execLine);
    if (arguments.isEmpty()) {
        return arguments;
    }

    QStringList cleaned;
    cleaned.reserve(arguments.size());
    for (QString argument : arguments) {
        if (QRegularExpression(QStringLiteral("^%[fFuUdDnNickvm]$")).match(argument).hasMatch()) {
            continue;
        }
        argument.replace(QRegularExpression(QStringLiteral("%[fFuUdDnNickvm]")), QString());
        argument.replace(QStringLiteral("%%"), QStringLiteral("%"));
        if (!argument.isEmpty()) {
            cleaned.append(argument);
        }
    }
    return cleaned;
}
}

ExplorerController::ExplorerController(QObject *parent)
    : QObject(parent)
    , m_displayName(qEnvironmentVariable("USER", QStringLiteral("friend")))
    , m_currentPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation))
    , m_runtimeStatus(QStringLiteral("Starting SolOS runtime…"))
{
    startRuntime();

    if (!m_displayName.isEmpty()) {
        m_displayName[0] = m_displayName.at(0).toUpper();
    }

    const QString home = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    const QList<QPair<QString, QString>> knownPlaces {
        {QStringLiteral("Home"), home},
        {QStringLiteral("Documents"), QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)},
        {QStringLiteral("Downloads"), QDir(home).filePath(QStringLiteral("Downloads"))},
        {QStringLiteral("Pictures"), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)},
    };
    for (const auto &place : knownPlaces) {
        if (QFileInfo(place.second).isDir()) {
            m_places.append(QVariantMap {{QStringLiteral("name"), place.first}, {QStringLiteral("path"), place.second}});
        }
    }

    loadFiles();
    loadApps();

    m_runtimeTimer.setInterval(750);
    connect(&m_runtimeTimer, &QTimer::timeout, this, &ExplorerController::checkRuntime);
    m_runtimeTimer.start();
}

QString ExplorerController::displayName() const
{
    return m_displayName;
}

QString ExplorerController::currentPath() const
{
    return m_currentPath;
}

QVariantList ExplorerController::places() const
{
    return m_places;
}

QVariantList ExplorerController::files() const
{
    return m_files;
}

QVariantList ExplorerController::apps() const
{
    return m_apps;
}

QString ExplorerController::runtimeStatus() const
{
    return m_runtimeStatus;
}

void ExplorerController::openFolder(const QString &path)
{
    const QFileInfo directory(path);
    if (!directory.isDir() || !directory.isReadable()) {
        return;
    }
    m_currentPath = directory.canonicalFilePath();
    loadFiles();
    emit filesChanged();
}

void ExplorerController::openParentFolder()
{
    const QString parent = QDir(m_currentPath).absoluteFilePath(QStringLiteral(".."));
    if (parent != m_currentPath) {
        openFolder(parent);
    }
}

void ExplorerController::openItem(const QString &path, bool isDirectory)
{
    if (isDirectory) {
        openFolder(path);
        return;
    }
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void ExplorerController::launchApp(const QString &desktopFile)
{
    QSettings entry(desktopFile, QSettings::IniFormat);
    entry.beginGroup(QStringLiteral("Desktop Entry"));
    const QStringList command = desktopExecArguments(entry.value(QStringLiteral("Exec")).toString());
    entry.endGroup();
    if (command.isEmpty()) {
        return;
    }

    const QStringList arguments = command.mid(1);
    QProcess::startDetached(command.constFirst(), arguments);
}

void ExplorerController::checkRuntime()
{
    const QString socketPath = daemonSocketPath();
    if (!socketPath.isEmpty()) {
        QLocalSocket socket;
        socket.connectToServer(socketPath, QIODevice::ReadWrite);
        if (socket.waitForConnected(150)) {
            if (m_runtimeStatus != QStringLiteral("SolOS runtime is running")) {
                m_runtimeStatus = QStringLiteral("SolOS runtime is running");
                emit runtimeStatusChanged();
            }
            m_runtimeTimer.stop();
            return;
        }
    }

    ++m_runtimeChecks;
    if (m_runtimeChecks >= 12) {
        m_runtimeTimer.setInterval(4000);
        if (m_runtimeStatus != QStringLiteral("Runtime service not connected")) {
            m_runtimeStatus = QStringLiteral("Runtime service not connected");
            emit runtimeStatusChanged();
        }
    }
}

void ExplorerController::loadFiles()
{
    QDir directory(m_currentPath);
    const QFileInfoList items = directory.entryInfoList(
        QDir::AllEntries | QDir::NoDotAndDotDot | QDir::Readable,
        QDir::DirsFirst | QDir::Name | QDir::IgnoreCase);

    m_files.clear();
    for (const QFileInfo &item : items) {
        m_files.append(QVariantMap {
            {QStringLiteral("name"), item.fileName()},
            {QStringLiteral("path"), item.absoluteFilePath()},
            {QStringLiteral("isDirectory"), item.isDir()},
            {QStringLiteral("detail"), item.isDir()
                    ? QStringLiteral("Folder")
                    : QLocale().formattedDataSize(item.size())},
        });
    }
}

void ExplorerController::loadApps()
{
    QHash<QString, QString> desktopFiles;
    for (const QString &directory : desktopEntryDirectories()) {
        QDirIterator iterator(directory, {QStringLiteral("*.desktop")}, QDir::Files, QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            const QString path = iterator.next();
            const QString relativeId = QDir(directory).relativeFilePath(path);
            desktopFiles.insert(relativeId, path);
        }
    }

    QStringList ids = desktopFiles.keys();
    ids.sort(Qt::CaseInsensitive);
    m_apps.clear();
    for (const QString &id : ids) {
        QSettings entry(desktopFiles.value(id), QSettings::IniFormat);
        entry.beginGroup(QStringLiteral("Desktop Entry"));
        const QString name = entry.value(QStringLiteral("Name")).toString();
        const QString exec = entry.value(QStringLiteral("Exec")).toString();
        const bool hidden = entry.value(QStringLiteral("NoDisplay"), false).toBool()
            || entry.value(QStringLiteral("Hidden"), false).toBool();
        entry.endGroup();
        if (name.isEmpty() || exec.isEmpty() || hidden) {
            continue;
        }
        m_apps.append(QVariantMap {
            {QStringLiteral("name"), name},
            {QStringLiteral("desktopFile"), desktopFiles.value(id)},
        });
    }
}

void ExplorerController::startRuntime()
{
    QProcess::startDetached(QStringLiteral("systemctl"), {
        QStringLiteral("--user"),
        QStringLiteral("start"),
        QStringLiteral("solos-daemon.service"),
    });
}
