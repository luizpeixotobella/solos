#pragma once

#include <QString>

bool runtimeSnapshotAvailable(const QString &socketPath, int timeoutMs = 350);
