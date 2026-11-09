#include <QCoreApplication>
#include <QDebug>
#include "runtimebridge.h"
int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    const auto state = RuntimeBridge::loadSnapshotFromDaemon(qEnvironmentVariable("SOLOS_DAEMON_SOCKET"), 3000);
    if (!state.isValid || state.runtimeMode != QStringLiteral("runtime-intermediary") || state.hostRuntimeSummary.isEmpty()) return 1;
    qInfo() << "PASS native RuntimeBridge parsed the actual daemon host snapshot";
    return 0;
}
