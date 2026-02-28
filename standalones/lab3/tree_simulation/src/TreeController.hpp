#pragma once
#include <QObject>
#include <QVariantList>
#include <QVariantMap>
#include <memory>
#include "TreeSimulation.hpp"
#include "TreeConfig.hpp"

class TreeController : public QObject {
    Q_OBJECT

    Q_PROPERTY(int nodeCount      READ nodeCount      NOTIFY treeChanged)
    Q_PROPERTY(int leafCount      READ leafCount      NOTIFY treeChanged)
    Q_PROPERTY(int targetLeafIndex READ targetLeafIndex NOTIFY treeChanged)
    Q_PROPERTY(int currentTargetLeaf READ currentTargetLeaf NOTIFY treeChanged)
    Q_PROPERTY(int M              READ getM           NOTIFY treeChanged)
    Q_PROPERTY(int treeDepth      READ getDepth       NOTIFY treeChanged)
    Q_PROPERTY(QString configError    READ configError    NOTIFY treeChanged)
    Q_PROPERTY(bool configLoaded   READ configLoaded   NOTIFY treeChanged)

    Q_PROPERTY(int inlineM          READ inlineM    WRITE setInlineM    NOTIFY inlineParamsChanged)
    Q_PROPERTY(int inlineDepth      READ inlineDepth WRITE setInlineDepth NOTIFY inlineParamsChanged)
    Q_PROPERTY(double inlineStuckProb  READ inlineStuckProb WRITE setInlineStuckProb NOTIFY inlineParamsChanged)
    Q_PROPERTY(int inlineDist       READ inlineDist  WRITE setInlineDist  NOTIFY inlineParamsChanged)
    Q_PROPERTY(double inlineGeomR      READ inlineGeomR WRITE setInlineGeomR NOTIFY inlineParamsChanged)
    Q_PROPERTY(double inlineLeftBias   READ inlineLeftBias WRITE setInlineLeftBias NOTIFY inlineParamsChanged)
    Q_PROPERTY(int inlineTargetLeaf READ inlineTargetLeaf WRITE setInlineTargetLeaf NOTIFY inlineParamsChanged)

    Q_PROPERTY(int totalRuns      READ totalRuns      NOTIFY statsChanged)
    Q_PROPERTY(int targetHits     READ targetHits     NOTIFY statsChanged)
    Q_PROPERTY(double hitProbability READ hitProbability NOTIFY statsChanged)

    Q_PROPERTY(QVariantList lastPath          READ lastPath          NOTIFY pathChanged)
    Q_PROPERTY(bool lastReachedLeaf   READ lastReachedLeaf   NOTIFY pathChanged)
    Q_PROPERTY(bool lastHitTarget     READ lastHitTarget     NOTIFY pathChanged)
    Q_PROPERTY(int lastPathLength    READ lastPathLength    NOTIFY pathChanged)

public:
    explicit TreeController(QObject* parent = nullptr);

    int nodeCount() const { return m_sim ? m_sim->nodeCount() : 0; }
    int leafCount() const { return m_sim ? m_sim->leafCount() : 0; }
    int targetLeafIndex() const { return m_sim ? m_sim->targetLeafIndex() : -1; }
    int getM() const { return m_sim ? m_sim->config().M : m_inlineM; }

    int currentTargetLeaf() const {
        if (m_sim) return m_sim->config().targetLeaf;
        return m_inlineTargetLeaf;
    }

    int getDepth() const { return m_sim ? m_sim->config().depth : m_inlineDepth; }
    QString configError() const { return m_configError; }
    bool configLoaded() const { return m_configLoaded; }

    int inlineM() const { return m_inlineM; }
    int inlineDepth() const { return m_inlineDepth; }
    double inlineStuckProb() const { return m_inlineStuckProb; }
    int inlineDist() const { return (int)m_inlineDist; }
    double inlineGeomR() const { return m_inlineGeomR; }
    double inlineLeftBias() const { return m_inlineLeftBias; }
    int inlineTargetLeaf() const { return m_inlineTargetLeaf; }

    void setInlineM(int v);
    void setInlineDepth(int v);
    void setInlineStuckProb(double v);
    void setInlineDist(int v);
    void setInlineGeomR(double v);
    void setInlineLeftBias(double v);
    void setInlineTargetLeaf(int v);

    int totalRuns() const { return m_totalRuns; }
    int targetHits() const { return m_targetHits; }
    double hitProbability() const;

    QVariantList lastPath() const { return m_lastPath; }
    bool lastReachedLeaf() const { return m_lastReachedLeaf; }
    bool lastHitTarget() const { return m_lastHitTarget; }
    int lastPathLength() const { return m_lastPathLength; }

    Q_INVOKABLE void loadFile(const QString& path);
    Q_INVOKABLE void reloadFile();
    Q_INVOKABLE void unloadFile();

    Q_INVOKABLE void runOnce();
    Q_INVOKABLE void runBatch(int N);
    Q_INVOKABLE void resetStats();

    Q_INVOKABLE bool isLeaf(int idx) const;
    Q_INVOKABLE int parentOf(int idx) const;
    Q_INVOKABLE int childStart(int idx) const;
    Q_INVOKABLE double nodeStuckProb(int idx) const;
    Q_INVOKABLE QVariantList nodeChildWeights(int idx) const;
    Q_INVOKABLE QVariantList pathLengthHistogram() const;

    signals :
    
    void treeChanged();
    void statsChanged();
    void pathChanged();
    void inlineParamsChanged();

private:
    void buildFromInline();
    void buildFromConfig(const TreeConfig& cfg);
    void resetAccumulated();
    void applyWalkResult(const SimulationResult& res);

    std::unique_ptr<TreeSimulation> m_sim;

    int m_inlineM = 3;
    int m_inlineDepth = 3;
    double m_inlineStuckProb = 0.1;
    DistType m_inlineDist = DistType::Uniform;
    double m_inlineGeomR = 0.5;
    double m_inlineLeftBias = 0.6;
    int m_inlineTargetLeaf = 0;

    QString m_loadedPath;
    QString m_configError;
    bool m_configLoaded = false;

    int m_totalRuns = 0;
    int m_targetHits = 0;
    std::vector<int> m_pathLengthHist;

    QVariantList m_lastPath;
    bool m_lastReachedLeaf = false;
    bool m_lastHitTarget = false;
    int m_lastPathLength = 0;
};