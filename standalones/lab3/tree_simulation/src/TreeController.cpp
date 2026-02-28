#include "TreeController.hpp"
#include <QDebug>

TreeController::TreeController(QObject* parent) : QObject(parent) {
    buildFromInline();
}


void TreeController::buildFromInline() {
    TreeConfig cfg;
    cfg.mode = ConfigMode::Global;
    cfg.M = m_inlineM;
    cfg.depth = m_inlineDepth;
    cfg.targetLeaf = m_inlineTargetLeaf;
    cfg.globalStuckProb = m_inlineStuckProb;
    cfg.dist = m_inlineDist;
    cfg.geometricR = m_inlineGeomR;
    cfg.leftBiasedFirst = m_inlineLeftBias;
    buildFromConfig(cfg);
}

void TreeController::buildFromConfig(const TreeConfig& cfg) {
    try {
        m_sim = std::make_unique<TreeSimulation>(cfg);
        m_configError.clear();
    }
    catch (const std::exception& e) {
        m_sim.reset();
        m_configError = QString::fromStdString(e.what());
        qWarning() << "TreeController: build failed:" << m_configError;
    }
    resetAccumulated();
    emit treeChanged();
}

void TreeController::resetAccumulated() {
    m_totalRuns = 0;
    m_targetHits = 0;
    m_pathLengthHist.clear();
    m_lastPath.clear();
    m_lastReachedLeaf = false;
    m_lastHitTarget = false;
    m_lastPathLength = 0;
    emit statsChanged();
    emit pathChanged();
}


void TreeController::loadFile(const QString& path) {
    m_loadedPath = path;
    try {
        TreeConfig cfg = TreeConfig::fromFile(path.toStdString());
        m_configLoaded = true;
        m_inlineM = cfg.M;
        m_inlineDepth = cfg.depth;
        m_inlineTargetLeaf = cfg.targetLeaf;
        m_inlineStuckProb = cfg.globalStuckProb;
        m_inlineDist = cfg.dist;
        m_inlineGeomR = cfg.geometricR;
        m_inlineLeftBias = cfg.leftBiasedFirst;
        buildFromConfig(cfg);
        emit inlineParamsChanged();
        qDebug() << "Loaded config from" << path;
    }
    catch (const std::exception& e) {
        m_configLoaded = false;
        m_configError = QString::fromStdString(e.what());
        m_sim.reset();
        emit treeChanged();
        qWarning() << "loadFile failed:" << m_configError;
    }
}

void TreeController::reloadFile() {
    if (!m_loadedPath.isEmpty()) loadFile(m_loadedPath);
}

void TreeController::unloadFile() {
    m_loadedPath.clear();
    m_configLoaded = false;
    m_configError.clear();
    buildFromInline();
}


void TreeController::setInlineM(int v) {
    if (v < 2 || v == m_inlineM) return;
    m_inlineM = v;
    if (m_inlineTargetLeaf >= m_inlineM * m_inlineDepth) m_inlineTargetLeaf = 0;
    emit inlineParamsChanged();
    if (!m_configLoaded) buildFromInline();
}

void TreeController::setInlineDepth(int v) {
    if (v < 1 || v == m_inlineDepth) return;
    m_inlineDepth = v;
    emit inlineParamsChanged();
    if (!m_configLoaded) buildFromInline();
}

void TreeController::setInlineStuckProb(double v) {
    m_inlineStuckProb = v;
    emit inlineParamsChanged();
    if (!m_configLoaded) buildFromInline();
}

void TreeController::setInlineDist(int v) {
    m_inlineDist = static_cast<DistType>(v);
    emit inlineParamsChanged();
    if (!m_configLoaded) buildFromInline();
}

void TreeController::setInlineGeomR(double v) {
    m_inlineGeomR = v;
    emit inlineParamsChanged();
    if (!m_configLoaded) buildFromInline();
}

void TreeController::setInlineLeftBias(double v) {
    m_inlineLeftBias = v;
    emit inlineParamsChanged();
    if (!m_configLoaded) buildFromInline();
}

void TreeController::setInlineTargetLeaf(int v) {
    if (v < 0) return;
    if (m_sim&& v 
    >=
    m_sim->leafCount()
    )
    return;
    if (v == m_inlineTargetLeaf) return;
    m_inlineTargetLeaf = v;
    emit inlineParamsChanged();
    if (!m_configLoaded) buildFromInline();
}


double TreeController::hitProbability() const {
    return m_totalRuns > 0 ? (double)m_targetHits / m_totalRuns : 0.0;
}

void TreeController::applyWalkResult(const SimulationResult& res) {
    m_totalRuns++;
    if (res.hitTarget) m_targetHits++;
    int l = res.verticesTraversed;
    if ((int)m_pathLengthHist.size() <= l) m_pathLengthHist.resize(l + 1, 0);
    m_pathLengthHist[l]++;
}

void TreeController::runOnce() {
    if (!m_sim) return;
    auto res = m_sim->runOnce();
    applyWalkResult(res);

    m_lastPath.clear();
    for (const auto& s : res.path) {
        QVariantMap m;
        m["node"] = s.nodeIndex;
        m["stuck"] = s.stuck;
        m_lastPath.append(m);
    }
    m_lastReachedLeaf = res.reachedLeaf;
    m_lastHitTarget = res.hitTarget;
    m_lastPathLength = res.verticesTraversed;

    emit statsChanged();
    emit pathChanged();
}

void TreeController::runBatch(int N) {
    if (!m_sim) return;
    auto br = m_sim->runBatch(N);
    m_totalRuns += br.totalRuns;
    m_targetHits += br.targetHits;

    if (m_pathLengthHist.size() < br.pathLengthHist.size())
        m_pathLengthHist.resize(br.pathLengthHist.size(), 0);
    for (int i = 0; i < (int)br.pathLengthHist.size(); ++i)
        m_pathLengthHist[i] += br.pathLengthHist[i];

    m_lastPath.clear();
    m_lastReachedLeaf = false;
    m_lastHitTarget = false;
    m_lastPathLength = 0;

    emit statsChanged();
    emit pathChanged();
}

void TreeController::resetStats() {
    resetAccumulated();
}


bool TreeController::isLeaf(int i) const { return m_sim ? m_sim->isLeaf(i) : false; }
int TreeController::parentOf(int i) const { return m_sim ? m_sim->parent(i) : -1; }
int TreeController::childStart(int i) const { return m_sim ? m_sim->childStart(i) : -1; }
double TreeController::nodeStuckProb(int i) const { return m_sim ? m_sim->nodeStuckProb(i) : 0.0; }

QVariantList TreeController::nodeChildWeights(int idx) const {
    QVariantList r;
    if (!m_sim) return r;
    for (double w : m_sim->nodeWeights(idx)) r.append(w);
    return r;
}

QVariantList TreeController::pathLengthHistogram() const {
    QVariantList r;
    for (int i = 0; i < (int)m_pathLengthHist.size(); ++i) {
        QVariantMap e;
        e["length"] = i;
        e["count"] = m_pathLengthHist[i];
        r.append(e);
    }
    return r;
}