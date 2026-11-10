#include "walletreader.h"
#include "chainrpc.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonArray>
#include <QFile>
#include <QEventLoop>
#include <iostream>

#define CHECK(x) do { if (!(x)) { std::cerr << "Failed line " << __LINE__ << ": " #x "\n"; return 1; } } while (0)
int main(int argc, char **argv) {
    QCoreApplication app(argc, argv);
    QTemporaryDir home; qputenv("XDG_CONFIG_HOME", home.path().toUtf8());
    // Public synthetic fixtures, never an owner's account.
    const QString address = "0x1111111111111111111111111111111111111111";
    QTcpServer server; CHECK(server.listen(QHostAddress::LocalHost));
    const QString endpoint = QString("http://127.0.0.1:%1").arg(server.serverPort());
    QString mode = "success";
    int calls = 0;
    bool requestValid = true;
    QObject::connect(&server, &QTcpServer::newConnection, &app, [&] {
        while (server.hasPendingConnections()) {
            auto socket = server.nextPendingConnection();
            auto buffer = std::make_shared<QByteArray>();
            QObject::connect(socket, &QTcpSocket::readyRead, socket, [&, socket, buffer] {
                buffer->append(socket->readAll());
                int split = buffer->indexOf("\r\n\r\n"); if (split < 0) return;
                int length = -1;
                for (auto line : buffer->left(split).split('\n'))
                    if (line.toLower().startsWith("content-length:")) length = line.mid(15).trimmed().toInt();
                if (length < 0 || buffer->size() < split + 4 + length) return;
                const auto request = QJsonDocument::fromJson(buffer->mid(split + 4, length)).object();
                buffer->clear(); ++calls;
                const auto method = request.value("method").toString();
                const bool chain = method == "eth_chainId";
                requestValid &= request.value("jsonrpc") == "2.0" && (chain || method == "eth_getBalance");
                if (!chain) requestValid &= request.value("params").toArray() == QJsonArray{address, "latest"};
                if (mode == "timeout") return;
                QJsonObject response{{"jsonrpc", "2.0"}, {"id", request.value("id")},
                    {"result", chain ? (mode == "wrong-chain" ? "0x1" : "0x89") : "0xde0b6b3a7640000"}};
                if (mode == "bad-id") response.insert("id", 99);
                if (mode == "rpc-error") response.insert("error", QJsonObject{{"code", -32000}});
                if (!chain && mode == "bad-quantity") response.insert("result", "0x00");
                if (!chain && mode == "overflow") response.insert("result", "0x" + QString(65, 'f'));
                if (mode == "bad-version") response.insert("jsonrpc", "1.0");
                QByteArray body = mode == "malformed" ? QByteArray("{") : QJsonDocument(response).toJson();
                if (mode == "oversized") body = QByteArray(70000, 'x');
                const QByteArray status = mode == "http-error" ? "500 Error" : "200 OK";
                socket->write("HTTP/1.1 " + status + "\r\nContent-Type: application/json\r\nContent-Length: " + QByteArray::number(body.size()) + "\r\nConnection: close\r\n\r\n" + body);
                socket->disconnectFromHost();
            });
            QObject::connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
        }
    });
    QDateTime now = QDateTime::currentDateTimeUtc();
    WalletReader wallet(nullptr, [&] { return now; });
    CHECK(wallet.address().isEmpty() && wallet.balance() == "Unavailable");
    CHECK(!wallet.configure("invented", endpoint));
    CHECK(!wallet.configure("0x0000000000000000000000000000000000000000", endpoint));
    CHECK(!wallet.configure(address, "http://remote.invalid"));
    CHECK(!wallet.configure(address, "https://user:pass@example.com"));
    CHECK(wallet.configure(address, endpoint));
    QFile config(home.path() + "/solos/wallet.json");
    CHECK(config.exists());
    CHECK(!(config.permissions() & (QFile::ReadGroup | QFile::WriteGroup | QFile::ReadOther | QFile::WriteOther)));
    WalletReader restored; CHECK(restored.address() == address && restored.endpoint() == endpoint);
    CHECK(restored.balance() == "Unavailable");
    auto query = [&] {
        QEventLoop loop; bool completed = false;
        auto c = QObject::connect(&wallet, &WalletReader::changed, &loop, [&] {
            if (!wallet.busy()) { completed = true; loop.quit(); }
        });
        wallet.refresh();
        QTimer::singleShot(6500, &loop, &QEventLoop::quit); loop.exec();
        QObject::disconnect(c); return completed;
    };
    CHECK(query()); CHECK(wallet.balance() == "1" && calls == 2 && requestValid);
    now = now.addSecs(301);
    QEventLoop staleLoop;
    auto staleConnection = QObject::connect(&wallet, &WalletReader::changed, &staleLoop, &QEventLoop::quit);
    QTimer::singleShot(1500, &staleLoop, &QEventLoop::quit); staleLoop.exec();
    QObject::disconnect(staleConnection);
    CHECK(wallet.balance() == "Unavailable" && wallet.status().startsWith("Stale"));
    for (const auto &scenario : {"wrong-chain", "bad-id", "rpc-error", "bad-quantity", "overflow", "bad-version", "malformed", "oversized", "http-error", "timeout"}) {
        mode = "success"; CHECK(query()); CHECK(wallet.balance() != "Unavailable");
        mode = scenario; const int before = calls;
        CHECK(query()); CHECK(wallet.balance() == "Unavailable" && wallet.status().startsWith("RPC failed"));
        if (mode == "wrong-chain") CHECK(calls == before + 1);
    }
    mode = "success"; wallet.refresh(); CHECK(wallet.clear());
    QEventLoop loop; QTimer::singleShot(100, &loop, &QEventLoop::quit); loop.exec();
    CHECK(wallet.address().isEmpty() && wallet.balance() == "Unavailable" && !config.exists());
    WalletReader cleared; CHECK(cleared.address().isEmpty());
    CHECK(ChainRpc::result(R"({"jsonrpc":"2.0","id":1,"result":"0x01"})", 1) == "0x01");
    CHECK(ChainRpc::result(R"({"jsonrpc":"2.0","id":1,"error":{},"result":"0x01"})", 1).isEmpty());
    CHECK(ChainRpc::result(R"({"jsonrpc":"2.0","id":"1","result":"0x01"})", 1).isEmpty());
    CHECK(ChainRpc::nativeAmount("0x0") == "0");
    CHECK(ChainRpc::nativeAmount("0x1") == "0.000000000000000001");
    CHECK(ChainRpc::nativeAmount("0x" + QString(64, 'f')) == "115792089237316195423570985008687907853269984665640564039457.584007913129639935");
    CHECK(ChainRpc::quantity("0x0") && !ChainRpc::quantity("0x01"));
    std::cout << "Wallet persistence, validation, chain ID, exact balance, 10 RPC failures, stale-result clearing and in-flight clear passed.\n";
}
