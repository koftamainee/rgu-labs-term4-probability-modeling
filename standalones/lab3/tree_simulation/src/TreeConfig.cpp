#include "TreeConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <unordered_map>


std::vector<double> TreeConfig::uniformWeights(int M) {
    return std::vector<double>(M, 1.0 / M);
}

std::vector<double> TreeConfig::geometricWeights(int M, double r) {
    std::vector<double> w(M);
    double v = 1.0;
    for (int i = 0; i < M; ++i, v *= r) w[i] = v;
    normalise(w);
    return w;
}

std::vector<double> TreeConfig::leftBiasedWeights(int M, double firstW) {
    std::vector<double> w(M);
    w[0] = firstW;
    double rest = (1.0 - firstW) / std::max(1, M - 1);
    for (int i = 1; i < M; ++i) w[i] = rest;
    normalise(w);
    return w;
}

void TreeConfig::normalise(std::vector<double>& w) {
    double s = std::accumulate(w.begin(), w.end(), 0.0);
    if (s <= 0) throw std::runtime_error("Weight vector sums to zero");
    for (auto& v : w) v /= s;
}

static int ipow(int base, int exp) {
    int r = 1;
    for (int i = 0; i < exp; ++i) r *= base;
    return r;
}

void TreeConfig::buildLevelCache() const {
    if (!m_levelStart.empty()) return;
    m_levelStart.resize(depth + 2);
    int total = 0, count = 1;
    for (int l = 0; l <= depth; ++l) {
        m_levelStart[l] = total;
        total += count;
        count *= M;
    }
    m_levelStart[depth + 1] = total;
}

int TreeConfig::totalNodes() const {
    buildLevelCache();
    return m_levelStart[depth + 1];
}

int TreeConfig::leafCount() const {
    return ipow(M, depth);
}

bool TreeConfig::isLeaf(int idx) const {
    buildLevelCache();
    return idx >= m_levelStart[depth];
}

int TreeConfig::levelOf(int idx) const {
    buildLevelCache();
    for (int l = 0; l <= depth; ++l)
        if (idx < m_levelStart[l + 1]) return l;
    return depth;
}

NodeConfig TreeConfig::resolvedNodeConfig(int idx) const {
    buildLevelCache();

    if (mode == ConfigMode::Custom) {
        for (const auto& nc : nodeConfigs)
            if (nc.id == idx) return nc;
    }

    NodeConfig nc;
    nc.id = idx;
    nc.stuckProb = globalStuckProb;
    if (!isLeaf(idx)) {
        switch (dist) {
        case DistType::Uniform: nc.weights = uniformWeights(M);
            break;
        case DistType::Geometric: nc.weights = geometricWeights(M, geometricR);
            break;
        case DistType::LeftBiased: nc.weights = leftBiasedWeights(M, leftBiasedFirst);
            break;
        }
    }
    return nc;
}


static std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

static std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

TreeConfig TreeConfig::fromFile(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open: " + path);

    TreeConfig cfg;
    cfg.m_levelStart.clear();

    std::unordered_map<int, NodeConfig> customNodes;

    std::string line;
    while (std::getline(f, line)) {
        auto hash = line.find('#');
        if (hash != std::string::npos) line = line.substr(0, hash);
        line = trim(line);
        if (line.empty()) continue;

        std::istringstream ss(line);
        std::string key;
        ss >> key;
        key = toLower(key);

        if (key == "mode") {
            std::string v;
            ss >> v;
            cfg.mode = (toLower(v) == "custom") ? ConfigMode::Custom : ConfigMode::Global;
        }
        else if (key == "m") {
            ss >> cfg.M;
            if (cfg.M < 2) throw std::runtime_error("M must be >= 2");
        }
        else if (key == "depth") {
            ss >> cfg.depth;
            if (cfg.depth < 1) throw std::runtime_error("depth must be >= 1");
        }
        else if (key == "target_leaf") {
            ss >> cfg.targetLeaf;
        }
        else if (key == "stuck_prob") {
            ss >> cfg.globalStuckProb;
        }
        else if (key == "distribution") {
            std::string v;
            ss >> v;
            v = toLower(v);
            if (v == "uniform") cfg.dist = DistType::Uniform;
            else if (v == "geometric") cfg.dist = DistType::Geometric;
            else if (v == "left_biased") cfg.dist = DistType::LeftBiased;
            else throw std::runtime_error("Unknown distribution: " + v);
        }
        else if (key == "geometric_r") {
            ss >> cfg.geometricR;
        }
        else if (key == "left_biased_first") {
            ss >> cfg.leftBiasedFirst;
        }
        else if (key == "node") {
            NodeConfig nc;
            ss >> nc.id;
            std::string tok;
            while (ss >> tok) {
                tok = toLower(tok);
                if (tok == "stuck") {
                    ss >> nc.stuckProb;
                }
                else if (tok == "weights") {
                    double w;
                    while (ss >> w) nc.weights.push_back(w);
                }
            }
            if (!nc.weights.empty()) normalise(nc.weights);
            customNodes[nc.id] = nc;
        }
    }

    int leaves = ipow(cfg.M, cfg.depth);
    if (cfg.targetLeaf < 0 || cfg.targetLeaf >= leaves) cfg.targetLeaf = 0;

    if (cfg.mode == ConfigMode::Custom) {
        cfg.nodeConfigs.clear();
        cfg.buildLevelCache();
        int nc = cfg.totalNodes();
        for (int i = 0; i < nc; ++i) {
            if (customNodes.count(i)) {
                auto& n = customNodes[i];
                if ((int)n.weights.size() != cfg.M && !cfg.isLeaf(i)) {
                    n.weights = uniformWeights(cfg.M);
                }
                cfg.nodeConfigs.push_back(n);
            }
        }
    }

    return cfg;
}