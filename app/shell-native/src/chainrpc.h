#pragma once
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QUrl>

namespace ChainRpc {
inline bool address(const QString &s) {
    return QRegularExpression("^0x[0-9a-fA-F]{40}$").match(s).hasMatch()
        && s.toLower() != "0x0000000000000000000000000000000000000000";
}
inline bool endpoint(const QUrl &url) {
    const bool local = url.host() == "127.0.0.1" || url.host() == "localhost" || url.host() == "::1";
    return url.isValid() && !url.host().isEmpty() && url.userInfo().isEmpty()
        && !url.hasQuery() && !url.hasFragment()
        && (url.scheme() == "https" || (local && url.scheme() == "http"));
}
inline QString result(const QByteArray &payload, int id) {
    const auto doc = QJsonDocument::fromJson(payload);
    if (!doc.isObject()) return {};
    const auto obj = doc.object();
    if (obj.value("jsonrpc").toString() != "2.0" || !obj.value("id").isDouble()
        || obj.value("id").toDouble() != id || obj.contains("error")) return {};
    return obj.value("result").toString();
}
inline bool quantity(const QString &s) {
    return QRegularExpression("^0x(0|[1-9a-fA-F][0-9a-fA-F]{0,63})$").match(s).hasMatch();
}
// Exact 256-bit quantity formatting without a floating-point balance estimate.
inline QString nativeAmount(const QString &hex) {
    if (!quantity(hex)) return {};
    QString decimal = "0";
    for (const QChar digit : hex.mid(2).toLower()) {
        int carry = digit.isDigit() ? digit.digitValue() : digit.unicode() - 'a' + 10;
        for (int i = decimal.size() - 1; i >= 0; --i) {
            const int value = decimal[i].digitValue() * 16 + carry;
            decimal[i] = QChar('0' + value % 10); carry = value / 10;
        }
        while (carry) { decimal.prepend(QChar('0' + carry % 10)); carry /= 10; }
    }
    decimal = decimal.rightJustified(19, '0');
    decimal.insert(decimal.size() - 18, '.');
    while (decimal.endsWith('0')) decimal.chop(1);
    if (decimal.endsWith('.')) decimal.chop(1);
    return decimal;
}

}
