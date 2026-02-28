#pragma once
#include <random>
#include <vector>
#include "TreeConfig.hpp"

struct WalkStep {
    int nodeIndex;
    bool stuck;
};

struct SimulationResult {
    std::vector<WalkStep> path;
    bool reachedLeaf = false;
    bool hitTarget = false;
    int targetLeafIdx = -1;
    int endNodeIdx = -1;
    int verticesTraversed = 0;
};

class TreeSimulation {
public:
    explicit TreeSimulation(const TreeConfig& cfg);

    SimulationResult runOnce();

    struct BatchResult {
        int totalRuns = 0;
        int targetHits = 0;
        std::vector<int> pathLengthHist;
    };

    BatchResult runBatch(int N);

    const TreeConfig& config() const { return m_cfg; }
    int nodeCount() const { return m_cfg.totalNodes(); }
    int leafCount() const { return m_cfg.leafCount(); }
    bool isLeaf(int i) const { return m_cfg.isLeaf(i); }
    int parent(int i) const;
    int childStart(int i) const;
    int targetLeafIndex() const;

    double nodeStuckProb(int idx) const;
    std::vector<double> nodeWeights(int idx) const;

private:
    TreeConfig m_cfg;
    int m_targetAbsIdx;
    std::vector<NodeConfig> m_resolved;

    std::mt19937 m_rng;
    std::uniform_real_distribution<double> m_udist;

    int pickChild(int parentIdx);
};