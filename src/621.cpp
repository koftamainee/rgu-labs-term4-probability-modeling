#include <print>
#include "task_runner.hpp"

int main() {
  double p = 0.4;
  double p1 = 0.6;
  int n = 5;
  size_t simulations = 1e6;

  auto experiment = [p, p1, n](auto& rng) {
    std::bernoulli_distribution hit(p);

    int hits = 0;
    for (int i = 0; i < n; i++) {
      if (hit(rng)) { ++hits; }
    }

    if (hits == 0) { return false; }
    if (hits >= 2) { return true; }

    return std::bernoulli_distribution(p1)(rng);
  };

  std::print("Barrel explosion simulation\n");
  std::print("Hit probability:                {:.2f}\n", p);
  std::print("Explosion probability on 1 hit: {:.2f}\n", p1);
  std::print("Number of shots:                {}\n\n", n);

  TaskRunner runner;
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);
  std::print("P(barrel explodes)     = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(barrel survives)     = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}