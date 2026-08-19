#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

class GhostRuntime : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString presenceLabel READ presenceLabel NOTIFY stateChanged)
    Q_PROPERTY(QString modeLabel READ modeLabel NOTIFY stateChanged)
    Q_PROPERTY(QString thesisLabel READ thesisLabel NOTIFY stateChanged)
    Q_PROPERTY(QString intelligenceSummary READ intelligenceSummary NOTIFY stateChanged)
    Q_PROPERTY(QString webStatusLabel READ webStatusLabel NOTIFY stateChanged)
    Q_PROPERTY(QString researchQuery READ researchQuery NOTIFY stateChanged)
    Q_PROPERTY(QString researchSummary READ researchSummary NOTIFY stateChanged)
    Q_PROPERTY(QString onboardingTitle READ onboardingTitle NOTIFY stateChanged)
    Q_PROPERTY(QString onboardingBody READ onboardingBody NOTIFY stateChanged)
    Q_PROPERTY(QString onboardingUrl READ onboardingUrl NOTIFY stateChanged)
    Q_PROPERTY(QString onboardingStatus READ onboardingStatus NOTIFY stateChanged)
    Q_PROPERTY(QString intentsTitle READ intentsTitle NOTIFY stateChanged)
    Q_PROPERTY(QString intentsSummary READ intentsSummary NOTIFY stateChanged)
    Q_PROPERTY(QStringList intentLines READ intentLines NOTIFY stateChanged)
    Q_PROPERTY(QStringList pipelineLines READ pipelineLines NOTIFY stateChanged)
    Q_PROPERTY(QStringList citationLines READ citationLines NOTIFY stateChanged)
    Q_PROPERTY(QString initiationStatus READ initiationStatus NOTIFY stateChanged)
    Q_PROPERTY(QString initiationSummary READ initiationSummary NOTIFY stateChanged)
    Q_PROPERTY(QString initiationDatabasePath READ initiationDatabasePath NOTIFY stateChanged)
    Q_PROPERTY(QStringList knowledgeLines READ knowledgeLines NOTIFY stateChanged)
    Q_PROPERTY(QString languageSupportStatus READ languageSupportStatus NOTIFY stateChanged)
    Q_PROPERTY(QString languageSupportSummary READ languageSupportSummary NOTIFY stateChanged)
    Q_PROPERTY(QStringList languageSupportLines READ languageSupportLines NOTIFY stateChanged)
    Q_PROPERTY(QString readinessStatus READ readinessStatus NOTIFY stateChanged)
    Q_PROPERTY(QString readinessSummary READ readinessSummary NOTIFY stateChanged)
    Q_PROPERTY(QStringList readinessLines READ readinessLines NOTIFY stateChanged)
    Q_PROPERTY(QString requestClassifierTitle READ requestClassifierTitle NOTIFY stateChanged)
    Q_PROPERTY(QString requestClassifierSummary READ requestClassifierSummary NOTIFY stateChanged)
    Q_PROPERTY(QStringList requestClassificationLines READ requestClassificationLines NOTIFY stateChanged)
    Q_PROPERTY(QString actionTraceSummary READ actionTraceSummary NOTIFY stateChanged)
    Q_PROPERTY(QString routeExplanationSummary READ routeExplanationSummary NOTIFY stateChanged)
    Q_PROPERTY(QString resolutionStatus READ resolutionStatus NOTIFY stateChanged)
    Q_PROPERTY(QString resolutionSummary READ resolutionSummary NOTIFY stateChanged)
    Q_PROPERTY(QString resolutionSelectedId READ resolutionSelectedId NOTIFY stateChanged)
    Q_PROPERTY(QString resolutionCurrentStep READ resolutionCurrentStep NOTIFY stateChanged)
    Q_PROPERTY(int resolutionProgress READ resolutionProgress NOTIFY stateChanged)
    Q_PROPERTY(QStringList resolutionLines READ resolutionLines NOTIFY stateChanged)

public:
    explicit GhostRuntime(QObject *parent = nullptr);

    QString presenceLabel() const;
    QString modeLabel() const;
    QString thesisLabel() const;
    QString intelligenceSummary() const;
    QString webStatusLabel() const;
    QString researchQuery() const;
    QString researchSummary() const;
    QString onboardingTitle() const;
    QString onboardingBody() const;
    QString onboardingUrl() const;
    QString onboardingStatus() const;
    QString intentsTitle() const;
    QString intentsSummary() const;
    QStringList intentLines() const;
    QStringList pipelineLines() const;
    QStringList citationLines() const;
    QString initiationStatus() const;
    QString initiationSummary() const;
    QString initiationDatabasePath() const;
    QStringList knowledgeLines() const;
    QString languageSupportStatus() const;
    QString languageSupportSummary() const;
    QStringList languageSupportLines() const;
    QString readinessStatus() const;
    QString readinessSummary() const;
    QStringList readinessLines() const;
    QString requestClassifierTitle() const;
    QString requestClassifierSummary() const;
    QStringList requestClassificationLines() const;
    QString actionTraceSummary() const;
    QString routeExplanationSummary() const;
    QString resolutionStatus() const;
    QString resolutionSummary() const;
    QString resolutionSelectedId() const;
    QString resolutionCurrentStep() const;
    int resolutionProgress() const;
    QStringList resolutionLines() const;

    void setLabels(const QString &presence,
                   const QString &mode,
                   const QString &thesis,
                   const QString &intelligenceSummary,
                   const QString &webStatusLabel,
                   const QString &researchQuery,
                   const QString &researchSummary,
                   const QString &onboardingTitle,
                   const QString &onboardingBody,
                   const QString &onboardingUrl,
                   const QString &onboardingStatus,
                   const QString &intentsTitle,
                   const QString &intentsSummary,
                   const QStringList &intentLines,
                   const QStringList &pipelineLines,
                   const QStringList &citationLines,
                   const QString &initiationStatus,
                   const QString &initiationSummary,
                   const QString &initiationDatabasePath,
                   const QStringList &knowledgeLines,
                   const QString &languageSupportStatus,
                   const QString &languageSupportSummary,
                   const QStringList &languageSupportLines,
                   const QString &readinessStatus,
                   const QString &readinessSummary,
                   const QStringList &readinessLines,
                   const QString &requestClassifierTitle,
                   const QString &requestClassifierSummary,
                   const QStringList &requestClassificationLines,
                   const QString &actionTraceSummary,
                   const QString &routeExplanationSummary,
                   const QString &resolutionStatus,
                   const QString &resolutionSummary,
                   const QString &resolutionSelectedId,
                   const QString &resolutionCurrentStep,
                   int resolutionProgress,
                   const QStringList &resolutionLines);

signals:
    void stateChanged();

private:
    QString m_presenceLabel;
    QString m_modeLabel;
    QString m_thesisLabel;
    QString m_intelligenceSummary;
    QString m_webStatusLabel;
    QString m_researchQuery;
    QString m_researchSummary;
    QString m_onboardingTitle;
    QString m_onboardingBody;
    QString m_onboardingUrl;
    QString m_onboardingStatus;
    QString m_intentsTitle;
    QString m_intentsSummary;
    QStringList m_intentLines;
    QStringList m_pipelineLines;
    QStringList m_citationLines;
    QString m_initiationStatus;
    QString m_initiationSummary;
    QString m_initiationDatabasePath;
    QStringList m_knowledgeLines;
    QString m_languageSupportStatus;
    QString m_languageSupportSummary;
    QStringList m_languageSupportLines;
    QString m_readinessStatus;
    QString m_readinessSummary;
    QStringList m_readinessLines;
    QString m_requestClassifierTitle;
    QString m_requestClassifierSummary;
    QStringList m_requestClassificationLines;
    QString m_actionTraceSummary;
    QString m_routeExplanationSummary;
    QString m_resolutionStatus;
    QString m_resolutionSummary;
    QString m_resolutionSelectedId;
    QString m_resolutionCurrentStep;
    int m_resolutionProgress = 0;
    QStringList m_resolutionLines;
};
