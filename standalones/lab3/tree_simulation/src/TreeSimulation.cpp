#include "TreeSimulation.hpp"

static int ipow(int b, int e) {
    int r = 1;
    for (int i = 0; i < e; ++i) r *= b;
    return r;
}

TreeSimulation::TreeSimulation(const TreeConfig& cfg)
    : m_cfg(cfg), m_rng(std::random_device{}()), m_udist(0.0, 1.0) {
    int nc = cfg.totalNodes();
    m_resolved.reserve(nc);
    for (int i = 0; i < nc; ++i)
        m_resolved.push_back(cfg.resolvedNodeConfig(i));

    int leaves = cfg.leafCount();
    int tl = cfg.targetLeaf;
    if (tl < 0 || tl >= leaves) tl = 0;
    int levelStart = 0, count = 1;
    for (int l = 0; l < cfg.depth; ++l) {
        levelStart += count;
        count *= cfg.M;
    }
    m_targetAbsIdx = levelStart + tl;
}

int TreeSimulation::parent(int i) const {
    if (i == 0) return -1;
    int lv = m_cfg.levelOf(i);
    int ls = 0, c = 1;
    for (int l = 0; l < lv; ++l) {
        ls += c;
        c *= m_cfg.M;
    }
    int posInLevel = i - ls;
    int lsp = 0;
    c = 1;
    for (int l = 0; l < lv - 1; ++l) {
        lsp += c;
        c *= m_cfg.M;
    }
    return lsp + posInLevel / m_cfg.M;
}

int TreeSimulation::childStart(int i) const {
    if (m_cfg.isLeaf(i)) return -1;
    int lv = m_cfg.levelOf(i);
    int ls = 0, c = 1;
    for (int l = 0; l < lv; ++l) {
        ls += c;
        c *= m_cfg.M;
    }
    int posInLevel = i - ls;
    int lsNext = ls + c;
    return lsNext + posInLevel * m_cfg.M;
}

int TreeSimulation::targetLeafIndex() const { return m_targetAbsIdx; }

double TreeSimulation::nodeStuckProb(int idx) const {
    if (idx < 0 || idx >= (int)m_resolved.size()) return 0.0;
    return m_resolved[idx].stuckProb;
}

std::vector<double> TreeSimulation::nodeWeights(int idx) const {
    if (idx < 0 || idx >= (int)m_resolved.size()) return {};
    return m_resolved[idx].weights;
}

int TreeSimulation::pickChild(int parentIdx) {
    const auto& w = m_resolved[parentIdx].weights;
    double r = m_udist(m_rng);
    double cum = 0;
    int first = childStart(parentIdx);
    for (int i = 0; i < (int)w.size(); ++i) {
        cum += w[i];
        if (r <= cum) return first + i;
    }
    return first + (int)w.size() - 1;
}

SimulationResult TreeSimulation::runOnce() {
    SimulationResult res;
    res.targetLeafIdx = m_targetAbsIdx;

    int cur = 0;
    while (true) {
        res.path.push_back({cur, false});

        if (m_cfg.isLeaf(cur)) {
            res.reachedLeaf = true;
            res.endNodeIdx = cur;
            res.hitTarget = (cur == m_targetAbsIdx);
            break;
        }
        if (m_udist(m_rng) < m_resolved[cur].stuckProb) {
            res.path.back().stuck = true;
            res.endNodeIdx = cur;
            break;
        }
        cur = pickChild(cur);
    }
    res.verticesTraversed = (int)res.path.size();
    return res;
}

TreeSimulation::BatchResult TreeSimulation::runBatch(int N) {
    BatchResult br;
    br.totalRuns = N;
    br.pathLengthHist.assign(m_cfg.depth + 2, 0);

    for (int i = 0; i < N; ++i) {
        auto r = runOnce();
        if (r.hitTarget) br.targetHits++;
        int l = r.verticesTraversed;
        if (l < (int)br.pathLengthHist.size())
            br.pathLengthHist[l]++;
        else
            br.pathLengthHist.back()++;
    }
    return br;
}