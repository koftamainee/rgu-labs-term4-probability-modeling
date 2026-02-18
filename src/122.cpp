#include <algorithm>
#include <cstdlib>
#include <numeric>
#include <print>
#include <vector>

#include "task_runner.hpp"

struct Outcome {
  bool Ai;
  bool Aj;
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::print("Usage: {} <N>\n", argv[0]);
    return 1;
  }

  size_t N = std::stoul(argv[1]);
  size_t i = 1;
  size_t j = N;

  TaskRunner runner;

  auto experiment = [N, i, j](auto& rng) {
    std::vector<size_t> perm(N);
    std::iota(perm.begin(), perm.end(), 1);
    std::shuffle(perm.begin(), perm.end(), rng);

    bool Ai = perm[i - 1] == i;
    bool Aj = perm[j - 1] == j;

    return Outcome{Ai, Aj};
  };

  size_t simulations = 1e7;

  std::print("Permutation experiment (N = {})\n", N);
  std::print("Event Ai: element {} is at position {}\n", i, i);
  std::print("Event Aj: element {} is at position {}\n", j, j);
  auto results = runner.run(experiment, simulations);
  std::println();

  size_t cnt_Ai = 0;
  size_t cnt_Aj = 0;
  size_t cnt_AiAj = 0;
  size_t cnt_Ai_or_Aj = 0;

  for (const auto& r : results) {
    if (r.Ai) {
      cnt_Ai++;
    }
    if (r.Aj) {
      cnt_Aj++;
    }
    if (r.Ai && r.Aj) {
      cnt_AiAj++;
    }
    if (r.Ai || r.Aj) {
      cnt_Ai_or_Aj++;
    }
  }

  double pAi = double(cnt_Ai) / simulations;
  double pAj = double(cnt_Aj) / simulations;
  double pAiAj = double(cnt_AiAj) / simulations;
  double pUnion = double(cnt_Ai_or_Aj) / simulations;

  std::print("P(Ai)        = {:.6f}\n", pAi);
  std::print("P(Aj)        = {:.6f}\n", pAj);
  std::print("P(Ai * Aj)   = {:.6f}\n", pAiAj);
  std::print("P(Ai + Aj)   = {:.6f}\n", pUnion);

  std::print(
      "\nCheck of addition theorem using empirical probabilities:\n"
      "P(Ai) + P(Aj) − P(Ai * Aj) = {:.6f}\n",
      pAi + pAj - pAiAj);

  std::print("\nDifference |P(Ai + Aj) − (P(Ai)+P(Aj)−P(Ai*Aj))| = {:.8f}\n",
             std::abs(pUnion - (pAi + pAj - pAiAj)));
}
