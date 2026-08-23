#include "runtimebridge.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocalSocket>

namespace {
QuickActionEntry parseQuickAction(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("title")).toString(),
        object.value(QStringLiteral("subtitle")).toString(),
        object.value(QStringLiteral("description")).toString()
    };
}

ActivityFeedEntry parseActivityEntry(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("title")).toString(),
        object.value(QStringLiteral("detail")).toString(),
        object.value(QStringLiteral("status")).toString()
    };
}

ApprovalQueueEntry parseApprovalEntry(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("id")).toString(),
        object.value(QStringLiteral("title")).toString(),
        object.value(QStringLiteral("description")).toString(),
        object.value(QStringLiteral("requestedBy")).toString(),
        object.value(QStringLiteral("capability")).toString(),
        object.value(QStringLiteral("scope")).toString(),
        object.value(QStringLiteral("risk")).toString(),
        object.value(QStringLiteral("status")).toString(),
        object.value(QStringLiteral("createdAt")).toString()
    };
}

AppRegistryEntry parseAppEntry(const QJsonObject &object)
{
    return {
        object.value(QStringLiteral("name")).toString(),
        object.value(QStringLiteral("subtitle")).toString(),
        object.value(QStringLiteral("description")).toString()
    };
}

QStringList parseGhostPipelineLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString name = object.value(QStringLiteral("name")).toString();
        const QString status = object.value(QStringLiteral("status")).toString();
        const QString detail = object.value(QStringLiteral("detail")).toString();
        lines.append(QStringLiteral("%1 [%2] %3").arg(name, status, detail));
    }
    return lines;
}

QStringList parseGhostIntentLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString name = object.value(QStringLiteral("name")).toString();
        const QString status = object.value(QStringLiteral("status")).toString();
        const QString reason = object.value(QStringLiteral("reason")).toString();
        const QString nextAction = object.value(QStringLiteral("nextAction")).toString();
        lines.append(QStringLiteral("%1 [%2]\n%3\nNext: %4").arg(name, status, reason, nextAction));
    }
    return lines;
}

QStringList parseGhostCitationLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString title = object.value(QStringLiteral("title")).toString();
        const QString url = object.value(QStringLiteral("url")).toString();
        const QString snippet = object.value(QStringLiteral("snippet")).toString();
        lines.append(QStringLiteral("%1\n%2\n%3").arg(title, url, snippet));
    }
    return lines;
}

QStringList parseGhostKnowledgeLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString name = object.value(QStringLiteral("name")).toString();
        const QString status = object.value(QStringLiteral("status")).toString();
        const QString summary = object.value(QStringLiteral("summary")).toString();
        const int sourceCount = object.value(QStringLiteral("sourceCount")).toInt();
        lines.append(QStringLiteral("%1 [%2 · %3 sources]\n%4").arg(name, status, QString::number(sourceCount), summary));
    }
    return lines;
}

QStringList parseGhostReadinessLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString name = object.value(QStringLiteral("name")).toString();
        const QString status = object.value(QStringLiteral("status")).toString();
        const QString evidence = object.value(QStringLiteral("evidence")).toString();
        const QString nextAction = object.value(QStringLiteral("nextAction")).toString();
        lines.append(QStringLiteral("%1 [%2]\n%3\nNext: %4").arg(name, status, evidence, nextAction));
    }
    return lines;
}

QStringList parseGhostRequestClassificationLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString name = object.value(QStringLiteral("name")).toString();
        const QString status = object.value(QStringLiteral("status")).toString();
        const QString safetyLevel = object.value(QStringLiteral("safetyLevel")).toString();
        const QString requiredTools = object.value(QStringLiteral("requiredTools")).toString();
        const QString approvalNeeds = object.value(QStringLiteral("approvalNeeds")).toString();
        const QString quotaCost = object.value(QStringLiteral("quotaCost")).toString();
        const QString route = object.value(QStringLiteral("route")).toString();
        lines.append(QStringLiteral("%1 [%2 · %3]\nTools: %4\nApproval: %5\nQuota: %6\nRoute: %7")
            .arg(name, status, safetyLevel, requiredTools, approvalNeeds, quotaCost, route));
    }
    return lines;
}

QString summarizeGhostActionTrace(const QJsonObject &object)
{
    if (object.isEmpty()) {
        return QStringLiteral("No Ghost action trace is present in the runtime snapshot.");
    }

    return QStringLiteral("%1\nRequest: %2\nData: %3\nTarget: %4\nRoute: %5\nOutcome: %6\nQuota: %7\nApproval: %8")
        .arg(object.value(QStringLiteral("traceId")).toString(),
             object.value(QStringLiteral("request")).toString(),
             object.value(QStringLiteral("data")).toString(),
             object.value(QStringLiteral("resultTarget")).toString(),
             object.value(QStringLiteral("algorithmRoute")).toString(),
             object.value(QStringLiteral("outcome")).toString(),
             object.value(QStringLiteral("quotaCost")).toString(),
             object.value(QStringLiteral("approvalRequired")).toString());
}

QString summarizeGhostRouteExplanation(const QJsonObject &object)
{
    if (object.isEmpty()) {
        return QStringLiteral("No Ghost route explanation is present in the runtime snapshot.");
    }

    return QStringLiteral("%1 -> %2 [%3]\n%4\nApproval: %5\nQuota: %6\nNext: %7")
        .arg(object.value(QStringLiteral("selectedClass")).toString(),
             object.value(QStringLiteral("selectedRoute")).toString(),
             object.value(QStringLiteral("safetyLevel")).toString(),
             object.value(QStringLiteral("explanation")).toString(),
             object.value(QStringLiteral("approvalPolicy")).toString(),
             object.value(QStringLiteral("quotaPolicy")).toString(),
             object.value(QStringLiteral("nextStep")).toString());
}

QStringList parseGhostResolutionLines(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QJsonObject object = value.toObject();
        const QString id = object.value(QStringLiteral("id")).toString();
        const QString title = object.value(QStringLiteral("title")).toString();
        const QString status = object.value(QStringLiteral("status")).toString();
        const QString readiness = object.value(QStringLiteral("readiness")).toString();
        const int progress = object.value(QStringLiteral("progress")).toInt();
        const QString target = object.value(QStringLiteral("targetOutcome")).toString();
        const QString result = object.value(QStringLiteral("resultSummary")).toString();
        const int evidenceCount = object.value(QStringLiteral("evidence")).toArray().size();

        QStringList steps;
        for (const QJsonValue &stepValue : object.value(QStringLiteral("steps")).toArray()) {
            const QJsonObject step = stepValue.toObject();
            steps.append(QStringLiteral("%1 [%2]")
                .arg(step.value(QStringLiteral("title")).toString(),
                     step.value(QStringLiteral("status")).toString()));
        }

        lines.append(QStringLiteral("%1 [%2 · %3 · %4%]\nID: %5\nTarget: %6\nResult: %7\nEvidence: %8\n%9")
            .arg(title, status, readiness, QString::number(progress), id, target, result,
                 QString::number(evidenceCount), steps.join(QStringLiteral(" -> "))));
    }
    return lines;
}

QStringList parseStringArray(const QJsonArray &array);

QStringList parseGhostAuditLines(const QJsonObject &audit)
{
    QStringList lines;
    if (audit.isEmpty()) {
        return lines;
    }
    const QJsonObject classification = audit.value(QStringLiteral("classification")).toObject();
    const QStringList scopes = parseStringArray(classification.value(QStringLiteral("detectedScopes")).toArray());
    lines.append(QStringLiteral("Class: %1 · risk: %2\nScopes: %3\nInput execution: %4")
        .arg(classification.value(QStringLiteral("requestClass")).toString(),
             classification.value(QStringLiteral("risk")).toString(),
             scopes.join(QStringLiteral(", ")),
             classification.value(QStringLiteral("embeddedInputExecution")).toBool()
                 ? QStringLiteral("enabled")
                 : QStringLiteral("disabled — text remains inert data")));

    for (const QJsonValue &value : audit.value(QStringLiteral("steps")).toArray()) {
        const QJsonObject step = value.toObject();
        lines.append(QStringLiteral("%1 [%2]\nCapability: %3\n%4")
            .arg(step.value(QStringLiteral("title")).toString(),
                 step.value(QStringLiteral("status")).toString(),
                 step.value(QStringLiteral("capability")).toString(),
                 step.value(QStringLiteral("result")).toString()));
    }
    for (const QJsonValue &value : audit.value(QStringLiteral("evidence")).toArray()) {
        const QJsonObject evidence = value.toObject();
        lines.append(QStringLiteral("Evidence · %1\n%2")
            .arg(evidence.value(QStringLiteral("label")).toString(),
                 evidence.value(QStringLiteral("detail")).toString()));
    }
    return lines;
}

QStringList parseStringArray(const QJsonArray &array)
{
    QStringList lines;
    for (const QJsonValue &value : array) {
        const QString line = value.toString();
        if (!line.isEmpty()) {
            lines.append(line);
        }
    }
    return lines;
}
}

RuntimeSnapshotData RuntimeBridge::loadSnapshot(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }
    return parseSnapshot(file.readAll());
}

RuntimeSnapshotData RuntimeBridge::loadSnapshotFromDaemon(const QString &socketPath, int timeoutMs)
{
    QLocalSocket socket;
    socket.connectToServer(socketPath, QIODevice::ReadWrite);
    if (!socket.waitForConnected(timeoutMs)) {
        return {};
    }

    const QByteArray request("{\"id\":\"native-shell-snapshot\",\"method\":\"snapshot.get\"}\n");
    if (socket.write(request) != request.size() || !socket.waitForBytesWritten(timeoutMs)
        || !socket.waitForReadyRead(timeoutMs)) {
        return {};
    }

    QByteArray responsePayload = socket.readLine();
    while (!responsePayload.endsWith('\n') && socket.waitForReadyRead(timeoutMs)) {
        responsePayload += socket.readLine();
    }
    const QJsonDocument response = QJsonDocument::fromJson(responsePayload);
    if (!response.isObject() || !response.object().value(QStringLiteral("ok")).toBool()) {
        return {};
    }
    return parseSnapshot(QJsonDocument(response.object().value(QStringLiteral("result")).toObject()).toJson(QJsonDocument::Compact));
}

RuntimeSnapshotData RuntimeBridge::parseSnapshot(const QByteArray &payload)
{
    RuntimeSnapshotData snapshot;

    const auto document = QJsonDocument::fromJson(payload);
    if (!document.isObject()) {
        return snapshot;
    }

    const QJsonObject root = document.object();
    const QJsonObject home = root.value(QStringLiteral("home")).toObject();
    const QJsonObject ghost = root.value(QStringLiteral("ghost")).toObject();
    const QJsonObject heartPass = root.value(QStringLiteral("heartPass")).toObject();
    const QJsonObject heartPassQuotaLayer = heartPass.value(QStringLiteral("quotaLayer")).toObject();
    const QJsonObject systemStatus = root.value(QStringLiteral("systemStatus")).toObject();
    const QJsonObject lastResearch = ghost.value(QStringLiteral("lastResearch")).toObject();
    const QJsonObject initiation = ghost.value(QStringLiteral("initiation")).toObject();
    const QJsonObject knowledge = ghost.value(QStringLiteral("knowledge")).toObject();
    const QJsonObject languageSupport = ghost.value(QStringLiteral("languageSupport")).toObject();
    const QJsonObject operationalReadiness = ghost.value(QStringLiteral("operationalReadiness")).toObject();
    const QJsonObject requestClassifier = ghost.value(QStringLiteral("requestClassifier")).toObject();
    const QJsonObject actionTrace = ghost.value(QStringLiteral("actionTrace")).toObject();
    const QJsonObject routeExplanation = ghost.value(QStringLiteral("routeExplanation")).toObject();
    const QJsonObject resolutionLoop = ghost.value(QStringLiteral("resolutionLoop")).toObject();
    const QJsonObject auditChallenge = ghost.value(QStringLiteral("auditChallenge")).toObject();

    snapshot.sessionLabel = root.value(QStringLiteral("sessionLabel")).toString();
    snapshot.systemLabel = root.value(QStringLiteral("systemLabel")).toString();
    snapshot.walletLabel = root.value(QStringLiteral("walletLabel")).toString();
    snapshot.agentStatus = root.value(QStringLiteral("agentStatus")).toString();
    snapshot.runtimeMode = root.value(QStringLiteral("runtimeMode")).toString();
    snapshot.runtimeSource = root.value(QStringLiteral("runtimeSource")).toString();
    snapshot.runtimeRole = root.value(QStringLiteral("runtimeRole")).toString();
    snapshot.mediationStatus = root.value(QStringLiteral("mediationStatus")).toString();

    snapshot.summaryTitle = home.value(QStringLiteral("summaryTitle")).toString();
    snapshot.summarySubtitle = home.value(QStringLiteral("summarySubtitle")).toString();
    snapshot.summaryBody = home.value(QStringLiteral("summaryBody")).toString();
    snapshot.nextActionTitle = home.value(QStringLiteral("nextActionTitle")).toString();
    snapshot.nextActionSubtitle = home.value(QStringLiteral("nextActionSubtitle")).toString();
    snapshot.nextActionBody = home.value(QStringLiteral("nextActionBody")).toString();

    snapshot.ghostPresenceLabel = ghost.value(QStringLiteral("presenceLabel")).toString();
    snapshot.ghostModeLabel = ghost.value(QStringLiteral("modeLabel")).toString();
    snapshot.ghostThesisLabel = ghost.value(QStringLiteral("thesisLabel")).toString();
    snapshot.ghostIntelligenceSummary = ghost.value(QStringLiteral("intelligenceSummary")).toString();
    snapshot.ghostWebStatusLabel = ghost.value(QStringLiteral("webStatusLabel")).toString();
    snapshot.ghostResearchQuery = lastResearch.value(QStringLiteral("query")).toString();
    snapshot.ghostResearchSummary = lastResearch.value(QStringLiteral("summary")).toString();
    snapshot.ghostOnboardingTitle = ghost.value(QStringLiteral("onboardingTitle")).toString();
    snapshot.ghostOnboardingBody = ghost.value(QStringLiteral("onboardingBody")).toString();
    snapshot.ghostOnboardingUrl = ghost.value(QStringLiteral("onboardingUrl")).toString();
    snapshot.ghostOnboardingStatus = ghost.value(QStringLiteral("onboardingStatus")).toString();
    snapshot.ghostIntentsTitle = ghost.value(QStringLiteral("intentsTitle")).toString();
    snapshot.ghostIntentsSummary = ghost.value(QStringLiteral("intentsSummary")).toString();
    snapshot.ghostIntentLines = parseGhostIntentLines(ghost.value(QStringLiteral("intents")).toArray());
    snapshot.ghostPipelineLines = parseGhostPipelineLines(ghost.value(QStringLiteral("pipelineStages")).toArray());
    snapshot.ghostCitationLines = parseGhostCitationLines(lastResearch.value(QStringLiteral("citations")).toArray());
    snapshot.ghostInitiationStatus = initiation.value(QStringLiteral("status")).toString();
    snapshot.ghostInitiationSummary = initiation.value(QStringLiteral("summary")).toString();
    snapshot.ghostInitiationDatabasePath = initiation.value(QStringLiteral("databasePath")).toString();
    snapshot.ghostKnowledgeLines = parseGhostKnowledgeLines(knowledge.value(QStringLiteral("topics")).toArray());
    snapshot.ghostLanguageSupportStatus = languageSupport.value(QStringLiteral("status")).toString();
    snapshot.ghostLanguageSupportSummary = languageSupport.value(QStringLiteral("summary")).toString();
    const QStringList languageNames = parseStringArray(languageSupport.value(QStringLiteral("primaryLanguages")).toArray());
    const QStringList languagePrinciples = parseStringArray(languageSupport.value(QStringLiteral("operatingPrinciples")).toArray());
    if (!languageNames.isEmpty()) {
        snapshot.ghostLanguageSupportLines.append(QStringLiteral("Primary language coverage: %1").arg(languageNames.join(QStringLiteral(", "))));
    }
    for (const QString &principle : languagePrinciples) {
        snapshot.ghostLanguageSupportLines.append(QStringLiteral("• %1").arg(principle));
    }
    snapshot.ghostReadinessStatus = operationalReadiness.value(QStringLiteral("status")).toString();
    snapshot.ghostReadinessSummary = operationalReadiness.value(QStringLiteral("summary")).toString();
    snapshot.ghostReadinessLines = parseGhostReadinessLines(operationalReadiness.value(QStringLiteral("pillars")).toArray());
    snapshot.ghostRequestClassifierTitle = requestClassifier.value(QStringLiteral("title")).toString();
    const QString requestClassifierSummary = requestClassifier.value(QStringLiteral("summary")).toString();
    const QString exampleRequest = requestClassifier.value(QStringLiteral("exampleRequest")).toString();
    snapshot.ghostRequestClassifierSummary = exampleRequest.isEmpty()
        ? requestClassifierSummary
        : QStringLiteral("%1\nExample: %2").arg(requestClassifierSummary, exampleRequest);
    snapshot.ghostRequestClassificationLines = parseGhostRequestClassificationLines(requestClassifier.value(QStringLiteral("classes")).toArray());
    snapshot.ghostActionTraceSummary = summarizeGhostActionTrace(actionTrace);
    snapshot.ghostRouteExplanationSummary = summarizeGhostRouteExplanation(routeExplanation);
    snapshot.ghostResolutionSummary = resolutionLoop.value(QStringLiteral("summary")).toString();
    snapshot.ghostResolutionSelectedId = resolutionLoop.value(QStringLiteral("selectedId")).toString();
    snapshot.ghostResolutionLines = parseGhostResolutionLines(resolutionLoop.value(QStringLiteral("resolutions")).toArray());
    const QJsonArray resolutions = resolutionLoop.value(QStringLiteral("resolutions")).toArray();
    for (const QJsonValue &value : resolutions) {
        const QJsonObject resolution = value.toObject();
        if (resolution.value(QStringLiteral("id")).toString() == snapshot.ghostResolutionSelectedId) {
            snapshot.ghostResolutionStatus = resolution.value(QStringLiteral("status")).toString();
            snapshot.ghostResolutionCurrentStep = resolution.value(QStringLiteral("currentStep")).toString();
            snapshot.ghostResolutionProgress = resolution.value(QStringLiteral("progress")).toInt();
            break;
        }
    }
    if (snapshot.ghostResolutionStatus.isEmpty()) {
        snapshot.ghostResolutionStatus = QStringLiteral("ready-for-selection");
    }

    snapshot.ghostAuditSummary = auditChallenge.value(QStringLiteral("summary")).toString();
    snapshot.ghostAuditActiveId = auditChallenge.value(QStringLiteral("activeId")).toString();
    const QJsonArray audits = auditChallenge.value(QStringLiteral("audits")).toArray();
    for (const QJsonValue &value : audits) {
        const QJsonObject audit = value.toObject();
        if (audit.value(QStringLiteral("id")).toString() != snapshot.ghostAuditActiveId) {
            continue;
        }
        const QJsonObject classification = audit.value(QStringLiteral("classification")).toObject();
        snapshot.ghostAuditStatus = audit.value(QStringLiteral("status")).toString();
        snapshot.ghostAuditInput = audit.value(QStringLiteral("input")).toString();
        snapshot.ghostAuditInputSha256 = audit.value(QStringLiteral("inputSha256")).toString();
        snapshot.ghostAuditRequestClass = classification.value(QStringLiteral("requestClass")).toString();
        snapshot.ghostAuditRisk = classification.value(QStringLiteral("risk")).toString();
        snapshot.ghostAuditRoute = classification.value(QStringLiteral("selectedRoute")).toString();
        snapshot.ghostAuditCurrentStep = audit.value(QStringLiteral("currentStep")).toString();
        snapshot.ghostAuditProgress = audit.value(QStringLiteral("progress")).toInt();
        snapshot.ghostAuditArtifactPath = audit.value(QStringLiteral("artifactPath")).toString();
        snapshot.ghostAuditReceiptPath = audit.value(QStringLiteral("receiptPath")).toString();
        snapshot.ghostAuditLines = parseGhostAuditLines(audit);
        break;
    }
    if (snapshot.ghostAuditStatus.isEmpty()) {
        snapshot.ghostAuditStatus = QStringLiteral("waiting-for-input");
        snapshot.ghostAuditCurrentStep = QStringLiteral("Submit an input for transparent classification");
    }

    snapshot.heartPassTitle = heartPass.value(QStringLiteral("title")).toString();
    snapshot.heartPassStatus = heartPass.value(QStringLiteral("status")).toString();
    snapshot.heartPassNetwork = heartPass.value(QStringLiteral("network")).toString();
    snapshot.heartPassTokenStandard = heartPass.value(QStringLiteral("tokenStandard")).toString();
    snapshot.heartPassContract = heartPass.value(QStringLiteral("contract")).toString();
    snapshot.heartPassTokenId = heartPass.value(QStringLiteral("tokenId")).toString();
    snapshot.heartPassOpenSeaUrl = heartPass.value(QStringLiteral("openSeaUrl")).toString();
    snapshot.heartPassSummary = heartPass.value(QStringLiteral("summary")).toString();
    snapshot.heartPassNextStep = heartPass.value(QStringLiteral("nextStep")).toString();
    snapshot.heartPassWalletAddress = heartPass.value(QStringLiteral("walletAddress")).toString();
    snapshot.heartPassOwnerAddress = heartPass.value(QStringLiteral("ownerAddress")).toString();
    snapshot.heartPassVerificationStatus = heartPass.value(QStringLiteral("verificationStatus")).toString();
    snapshot.heartPassLastCheckedAt = heartPass.value(QStringLiteral("lastCheckedAt")).toString();
    snapshot.heartPassConfigPath = heartPass.value(QStringLiteral("configPath")).toString();
    snapshot.heartPassCapabilityLines = parseStringArray(heartPass.value(QStringLiteral("capabilities")).toArray());
    snapshot.heartPassQuotaTitle = heartPassQuotaLayer.value(QStringLiteral("title")).toString();
    snapshot.heartPassQuotaStatus = heartPassQuotaLayer.value(QStringLiteral("status")).toString();
    snapshot.heartPassQuotaMode = heartPassQuotaLayer.value(QStringLiteral("mode")).toString();
    snapshot.heartPassQuotaPeriod = heartPassQuotaLayer.value(QStringLiteral("period")).toString();
    snapshot.heartPassQuotaIncludedQueries = heartPassQuotaLayer.value(QStringLiteral("includedQueries")).toInt();
    snapshot.heartPassQuotaUsedQueries = heartPassQuotaLayer.value(QStringLiteral("usedQueries")).toInt();
    snapshot.heartPassQuotaRemainingQueries = heartPassQuotaLayer.value(QStringLiteral("remainingQueries")).toInt();
    snapshot.heartPassQuotaFallback = heartPassQuotaLayer.value(QStringLiteral("fallback")).toString();
    snapshot.heartPassQuotaUsageSource = heartPassQuotaLayer.value(QStringLiteral("usageSource")).toString();
    snapshot.heartPassQuotaLastSync = heartPassQuotaLayer.value(QStringLiteral("lastSync")).toString();
    snapshot.heartPassQuotaResetPolicy = heartPassQuotaLayer.value(QStringLiteral("resetPolicy")).toString();
    snapshot.heartPassQuotaSummary = heartPassQuotaLayer.value(QStringLiteral("summary")).toString();
    snapshot.heartPassQuotaNextStep = heartPassQuotaLayer.value(QStringLiteral("nextStep")).toString();

    snapshot.hostRuntimeSummary = systemStatus.value(QStringLiteral("hostRuntimeSummary")).toString();
    snapshot.online = systemStatus.value(QStringLiteral("online")).toBool(false);
    snapshot.approvalsCount = systemStatus.value(QStringLiteral("approvalsCount")).toInt();
    snapshot.notificationsCount = systemStatus.value(QStringLiteral("notificationsCount")).toInt();

    const QJsonArray quickActions = root.value(QStringLiteral("quickActions")).toArray();
    for (const QJsonValue &value : quickActions) {
        snapshot.quickActions.append(parseQuickAction(value.toObject()));
    }

    const QJsonArray activityFeed = root.value(QStringLiteral("activityFeed")).toArray();
    for (const QJsonValue &value : activityFeed) {
        snapshot.activityFeed.append(parseActivityEntry(value.toObject()));
    }

    const QJsonArray approvals = root.value(QStringLiteral("approvals")).toArray();
    for (const QJsonValue &value : approvals) {
        snapshot.approvals.append(parseApprovalEntry(value.toObject()));
    }

    const QJsonArray apps = root.value(QStringLiteral("apps")).toArray();
    for (const QJsonValue &value : apps) {
        snapshot.apps.append(parseAppEntry(value.toObject()));
    }

    snapshot.isValid = !snapshot.sessionLabel.isEmpty();
    return snapshot;
}
