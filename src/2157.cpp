#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  const double p1 = 0.6;
  const double p2 = 0.7;
  const double p3 = 0.8;
  const double p4 = 0.9;

  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [&](auto& rng) {
    std::bernoulli_distribution d1(p1), d2(p2), d3(p3), d4(p4);
    return d1(rng) + d2(rng) + d3(rng) + d4(rng);
  };

  auto results = runner.run(experiment, simulations);
  auto counts = tally(results);

  double p_le_3 = 0.0;
  double p_ge_2 = 0.0;

  for (const auto& [k, cnt] : counts) {
    double freq = static_cast<double>(cnt) / simulations;

    std::println("P(X = {}) ≈ {:.6f}", k, freq);

    if (k <= 3) {
      p_le_3 += freq;
    }

    if (k >= 2) {
      p_ge_2 += freq;
    }
  }

  std::println();
  std::println("P(X <= 3) ≈ {:.6f}", p_le_3);
  std::println("P(X >= 2) ≈ {:.6f}", p_ge_2);
}
