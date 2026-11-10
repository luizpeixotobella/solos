#include "runtimehealth.h"

#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>

bool runtimeSnapshotAvailable(const QString &socketPath, int timeoutMs)
{
    constexpr qint64 maximumReplySize = 64 * 1024;
    QElapsedTimer deadline;
    deadline.start();
    QLocalSocket socket;
    socket.setReadBufferSize(maximumReplySize + 1);
    socket.connectToServer(socketPath, QIODevice::ReadWrite);
    if (!socket.waitForConnected(timeoutMs)) return false;
    socket.write("{\"method\":\"health.get\"}\n");
    socket.flush();
    // A stream read can deliver only part of the JSON response. The Rust
    // daemon may serialize it in several writes; wait for the line delimiter,
    // under one overall deadline rather than restarting it for every chunk.
    while (!socket.canReadLine()) {
        if (socket.bytesAvailable() > maximumReplySize) return false;
        const int remaining = timeoutMs - static_cast<int>(deadline.elapsed());
        if (remaining <= 0 || !socket.waitForReadyRead(remaining)) return false;
    }
    const QByteArray payload = socket.readLine(maximumReplySize + 1);
    if (!payload.endsWith('\n') || payload.size() > maximumReplySize) return false;
    const auto reply = QJsonDocument::fromJson(payload).object();
    return reply.value(QStringLiteral("ok")).toBool()
        && reply.value(QStringLiteral("result")).toObject()
            .value(QStringLiteral("snapshotAvailable")).toBool();
}
