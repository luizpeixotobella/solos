#include "ghostapiclient.h"
#include <QCoreApplication>
#include <QTimer>
#include <iostream>

int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    const bool live = app.arguments().contains("--live");
    if (!live) {
        qputenv("SOLOS_GHOST_API_URL", "http://example.com/v1");
        GhostApiClient client;
        if (client.configured()) return 1;
        std::cout << "Remote plaintext endpoint rejected.\n"; return 0;
    }
    GhostApiClient client;
    if (!client.configured()) { std::cerr << "Live client not configured.\n"; return 1; }
    bool sent = false;
    QObject::connect(&client, &GhostApiClient::changed, &app, [&] {
        if (!sent || client.busy()) return;
        if (!client.transcript().contains("Ghost: GHOST-NATIVE-OK")) { app.exit(1); return; }
        sent = false; client.clear();
        if (!client.transcript().isEmpty()) { app.exit(1); return; }
        std::cout << "Native Qt client: authenticated live chat and session clear passed.\n"; app.quit();
    });
    QTimer::singleShot(0, [&] { sent = true; client.send("Reply with exactly: GHOST-NATIVE-OK"); });
    QTimer::singleShot(185000, &app, [&] { app.exit(1); });
    return app.exec();
}
