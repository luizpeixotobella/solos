#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QFileInfo>

#include "explorercontroller.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("SolOS Explorer"));

    ExplorerController controller;
    QQmlApplicationEngine engine;
    engine.addImportPath(QStringLiteral("qrc:/qt/qml"));
    bool openedFolder = false;
    if (app.arguments().size() > 1) {
        const QUrl folder = QUrl::fromUserInput(app.arguments().at(1));
        if (folder.isLocalFile() && QFileInfo(folder.toLocalFile()).isDir()) {
            controller.openFolder(folder.toLocalFile());
            openedFolder = true;
        }
    }
    engine.rootContext()->setContextProperty(QStringLiteral("initialView"), openedFolder ? QStringLiteral("Files") : QStringLiteral("Home"));
    engine.rootContext()->setContextProperty(QStringLiteral("kioskMode"), qEnvironmentVariable("SOLOS_KIOSK") == QStringLiteral("1"));
    engine.rootContext()->setContextProperty(QStringLiteral("explorer"), &controller);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(QUrl(QStringLiteral("qrc:/qt/qml/SolOS/ExplorerPrototype/qml/Main.qml")));

    return app.exec();
}
