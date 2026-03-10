#include <fstream>
#include <iostream>
#include <vector>
#include <random>
#include <nlohmann/json.hpp>
#include "task_runner.hpp"

using json = nlohmann::json;

struct WalkExperiment {
  double p;
  int64_t s_plus;
  int64_t s_minus;
  uint64_t steps;

  template<class RNG>
  bool operator()(RNG& rng) {
    std::bernoulli_distribution step(p);
    int64_t pos = 0;

    for (uint64_t i = 0; i < steps; ++i) {
      if (step(rng))
        pos += s_plus;
      else
        pos -= s_minus;
    }

    return pos == 0;
  }
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <config.json>\n";
    return 1;
  }

  std::ifstream f(argv[1]);
  json j;
  f >> j;

  double p = j["p"];
  int64_t s_plus = j["s_plus"];
  int64_t s_minus = j["s_minus"];
  std::vector<uint64_t> Ns = j["N"].get<std::vector<uint64_t>>();
  size_t simulations = j["simulations"];

  TaskRunner runner;

  for (auto N : Ns) {
    WalkExperiment exp{p, s_plus, s_minus, N};

    auto results = runner.run(exp, simulations);

    size_t success = 0;
    for (auto r : results)
      if (r)
        success++;

    double prob = static_cast<double>(success) / simulations;

    std::cout << "N=" << N << " P=" << prob << "\n";
  }
}