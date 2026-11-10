#include "ghostapiclient.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <iostream>
#include <memory>

#define CHECK(x) do { if (!(x)) { std::cerr << "Failed line " << __LINE__ << ": " #x "\n"; return 1; } } while (0)
int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QTemporaryDir home; qputenv("XDG_CONFIG_HOME", home.path().toUtf8());
    qunsetenv("SOLOS_GHOST_API_URL"); qunsetenv("SOLOS_GHOST_API_KEY_FILE"); qunsetenv("SOLOS_GHOST_MODEL");
    const QString key(48, 'k'); // Synthetic fixture, never an owner's credential.
    QTcpServer server, redirect; CHECK(server.listen(QHostAddress::LocalHost)); CHECK(redirect.listen(QHostAddress::LocalHost));
    const QString endpoint = QString("http://127.0.0.1:%1/v1").arg(server.serverPort());
    QString mode = "success"; int calls = 0; bool validRequest = true; int messageCount = 0;
    QObject::connect(&server, &QTcpServer::newConnection, &app, [&] {
        while (server.hasPendingConnections()) {
            auto socket = server.nextPendingConnection(); auto buffer = std::make_shared<QByteArray>();
            QObject::connect(socket, &QTcpSocket::readyRead, socket, [&, socket, buffer] {
                buffer->append(socket->readAll());
                const int split = buffer->indexOf("\r\n\r\n"); if (split < 0) return;
                int length = -1; bool auth = false;
                for (auto line : buffer->left(split).split('\n')) {
                    if (line.toLower().startsWith("content-length:")) length = line.mid(15).trimmed().toInt();
                    if (line.startsWith("Authorization:")) auth = line.trimmed() == "Authorization: Bearer " + key.toUtf8();
                }
                if (length < 0 || buffer->size() < split + 4 + length) return;
                const auto request = QJsonDocument::fromJson(buffer->mid(split + 4, length)).object();
                validRequest &= auth && buffer->startsWith("POST /v1/chat/completions ")
                    && request.value("model") == "ghost-test" && request.value("stream") == false;
                messageCount = request.value("messages").toArray().size();
                buffer->clear(); ++calls;
                if (mode == "timeout") return;
                auto body = QJsonDocument(QJsonObject{{"choices", QJsonArray{QJsonObject{{"message", QJsonObject{{"role", "assistant"}, {"content", "GHOST-MOCK-OK"}}}}}}}).toJson();
                if (mode == "malformed") body = "{";
                if (mode == "empty") body = "{\"choices\":[]}";
                if (mode == "rpc-error") body = "{\"error\":{\"message\":\"private fixture detail\"}}";
                if (mode == "oversized") body = QByteArray(2 * 1024 * 1024 + 1, 'x');
                const auto status = mode == "http-error" ? QByteArray("401 Unauthorized") : mode == "redirect" ? QByteArray("307 Temporary Redirect") : QByteArray("200 OK");
                const auto location = mode == "redirect" ? "Location: http://127.0.0.1:" + QByteArray::number(redirect.serverPort()) + "/steal\r\n" : QByteArray();
                socket->write("HTTP/1.1 " + status + "\r\n" + location + "Content-Type: application/json\r\nContent-Length: " + QByteArray::number(body.size()) + "\r\nConnection: close\r\n\r\n" + body);
                socket->disconnectFromHost();
            });
            QObject::connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
        }
    });
    GhostApiClient client(nullptr, 300);
    CHECK(!client.configured() && client.endpoint().isEmpty());
    client.send("not configured"); CHECK(calls == 0);
    for (const auto &url : {"http://remote.invalid/v1", "https://user:pass@example.com/v1", "https://example.com/v1?token=x", "https://example.com/v1#fragment", "file:///tmp/ghost"})
        CHECK(!client.configure(url, key, "ghost-test"));
    CHECK(!client.configure(endpoint, "short", "ghost-test"));
    CHECK(!client.configure(endpoint, key + "\r\n", "ghost-test"));
    CHECK(!client.configure(endpoint, QString(4097, 'k'), "ghost-test"));
    CHECK(!client.configure(endpoint, key, ""));
    CHECK(!client.configure(endpoint, key, "bad model\n"));
    CHECK(!client.configure(endpoint, key, QString(129, 'm')));
    CHECK(client.configure(endpoint + "/", key, "ghost-test")); CHECK(calls == 0);
    QFile saved(home.path() + "/solos/ghost-api.json"); CHECK(saved.exists());
    CHECK(!(saved.permissions() & (QFile::ReadGroup | QFile::WriteGroup | QFile::ReadOther | QFile::WriteOther)));
    CHECK(!(QFileInfo(home.path() + "/solos").permissions() & (QFile::ReadGroup | QFile::ExeGroup | QFile::ReadOther | QFile::ExeOther)));
    GhostApiClient restored; CHECK(restored.configured() && restored.endpoint() == endpoint && restored.model() == "ghost-test");
    CHECK(restored.transcript().isEmpty() && calls == 0);
    CHECK(client.configure(endpoint, "", "ghost-test"));
    CHECK(!client.configure("https://different.invalid/v1", "", "ghost-test"));
    CHECK(client.endpoint() == endpoint && client.configured());
    auto send = [&](QString text) {
        QEventLoop loop; bool finished = false;
        auto c = QObject::connect(&client, &GhostApiClient::changed, &loop, [&] {
            if (!client.busy()) { finished = true; loop.quit(); }
        });
        client.send(text); QTimer::singleShot(2000, &loop, &QEventLoop::quit); loop.exec();
        QObject::disconnect(c); return finished;
    };
    CHECK(send("hello mock")); CHECK(validRequest && calls == 1 && client.transcript().contains("Ghost: GHOST-MOCK-OK"));
    const auto good = client.transcript();
    for (const auto &scenario : {"http-error", "redirect", "malformed", "empty", "rpc-error", "oversized", "timeout"}) {
        mode = scenario; CHECK(send("failure fixture"));
        CHECK(client.transcript() == good && client.status().startsWith("Ghost could not answer"));
        CHECK(!client.status().contains(key) && !client.status().contains("private fixture detail"));
    }
    CHECK(!redirect.hasPendingConnections());
    mode = "success"; client.clear(); CHECK(client.transcript().isEmpty() && client.configured());
    for (int i = 0; i < 11; ++i) CHECK(send(QString::number(i)));
    CHECK(client.transcript().count("Ghost: ") == 10 && messageCount == 21);
    const int before = calls; client.send(QString(4001, 'x')); CHECK(calls == before && !client.busy());
    mode = "timeout"; client.send("clear in flight"); CHECK(client.busy()); client.clear();
    CHECK(!client.busy() && client.transcript().isEmpty());
    mode = "success"; CHECK(send("after clear"));
    client.send("reset in flight"); CHECK(client.resetConfiguration());
    CHECK(!client.configured() && !client.busy() && client.transcript().isEmpty());
    CHECK(saved.open(QIODevice::ReadOnly)); CHECK(!saved.readAll().contains(key.toUtf8())); saved.close();
    QFile envKey(home.path() + "/env-key"); CHECK(envKey.open(QIODevice::WriteOnly)); envKey.setPermissions(QFile::ReadOwner | QFile::WriteOwner); envKey.write(key.toUtf8()); envKey.close();
    qputenv("SOLOS_GHOST_API_URL", endpoint.toUtf8()); qputenv("SOLOS_GHOST_API_KEY_FILE", envKey.fileName().toUtf8()); qputenv("SOLOS_GHOST_MODEL", "ghost-test");
    GhostApiClient disabled; CHECK(!disabled.configured());
    CHECK(saved.remove()); GhostApiClient fromEnv; CHECK(fromEnv.configured() && !saved.exists());
    CHECK(client.configure(endpoint, key, "ghost-test"));
    saved.setPermissions(QFile::ReadOwner | QFile::WriteOwner | QFile::ReadOther);
    GhostApiClient unsafe; CHECK(!unsafe.configured());
    saved.setPermissions(QFile::ReadOwner | QFile::WriteOwner);
    CHECK(saved.open(QIODevice::WriteOnly | QIODevice::Truncate)); saved.write("{"); saved.close();
    GhostApiClient corrupt; CHECK(!corrupt.configured());
    CHECK(client.configure(endpoint, key, "ghost-test"));
    CHECK(saved.remove()); CHECK(QFile::link(envKey.fileName(), saved.fileName()));
    CHECK(!client.configure(endpoint, key, "ghost-test"));
    CHECK(envKey.open(QIODevice::ReadOnly)); CHECK(envKey.readAll() == key.toUtf8());
    CHECK(saved.remove()); CHECK(client.resetConfiguration());
    QEventLoop drain; QTimer::singleShot(50, &drain, &QEventLoop::quit); drain.exec();
    CHECK(!client.configured() && client.transcript().isEmpty());
    std::cout << "Ghost local mock: setup validation, private persistence/reload, environment compatibility/reset, authenticated model routing, 7 failures, redirect refusal, history bounds and in-flight clear/reset passed. Not real inference.\n";
}
