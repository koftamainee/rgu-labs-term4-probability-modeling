#pragma once
#include <string>
#include <vector>

enum class ConfigMode { Global, Custom };

enum class DistType { Uniform, Geometric, LeftBiased };

struct NodeConfig {
    int id = 0;
    double stuckProb = 0.0;
    std::vector<double> weights;
};

struct TreeConfig {
    ConfigMode mode = ConfigMode::Global;
    int M = 2;
    int depth = 3;
    int targetLeaf = 0;

    double globalStuckProb = 0.1;
    DistType dist = DistType::Uniform;
    double geometricR = 0.5;
    double leftBiasedFirst = 0.6;

    std::vector<NodeConfig> nodeConfigs;

    static TreeConfig fromFile(const std::string& path);

    static std::vector<double> uniformWeights(int M);
    static std::vector<double> geometricWeights(int M, double r);
    static std::vector<double> leftBiasedWeights(int M, double firstW);
    static void normalise(std::vector<double>& w);

    NodeConfig resolvedNodeConfig(int idx) const;

    int totalNodes() const;
    int leafCount() const;
    bool isLeaf(int idx) const;
    int levelOf(int idx) const;

private:
    mutable std::vector<int> m_levelStart;
    void buildLevelCache() const;
};