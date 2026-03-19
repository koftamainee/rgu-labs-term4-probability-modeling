#include <cmath>
#include <print>
#include "task_runner.hpp"

static constexpr double LAMBDA = 1.94;
static constexpr int M_THRESHOLD = 2;

struct FamilyResult {
  int children;
  bool all_boys;
};

int main() {
  TaskRunner runner;
  constexpr size_t N = 2'000'000;
  constexpr int m = M_THRESHOLD;

  auto results = runner.run([](std::mt19937& rng) -> FamilyResult {
    std::poisson_distribution<int> poisson(LAMBDA);
    int k = poisson(rng);
    std::bernoulli_distribution boy(0.5);
    for (int i = 0; i < k; ++i)
      if (!boy(rng)) return {k, false};
    return {k, true};
}, N);

  size_t no_girls_total = 0;
  size_t no_girls_geq_m = 0;
  size_t geq_m_total = 0;

  for (const auto& r : results) {
    if (r.children >= m) geq_m_total++;
    if (r.all_boys) {
      no_girls_total++;
      if (r.children >= m) no_girls_geq_m++;
    }
  }

  double p_conditional = static_cast<double>(no_girls_geq_m) / no_girls_total;
  double p_unconditional = static_cast<double>(geq_m_total) / N;

  std::println("=== Results (m={}, lambda={}, N={}) ===", m, LAMBDA, N);
  std::println("");
  std::println("Conditional   P(K>={} | no girls):", m);
  std::println("  Empirical : {:.6f}", p_conditional);
  std::println("");
  std::println("Unconditional P(K>={}):", m);
  std::println("  Empirical : {:.6f}", p_unconditional);
  std::println("");
  std::println("Difference (conditional - unconditional): {:.6f}",
               p_conditional - p_unconditional);
  std::println("Ratio      (conditional / unconditional): {:.4f}",
               p_conditional / p_unconditional);

  return 0;
}