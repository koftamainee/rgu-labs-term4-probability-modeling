#include <cstdlib>
#include <print>
#include <random>

#include "task_runner.hpp"

int main() {
  int n = 6;
  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [n](auto& rng) {
    for (int i = 0; i < n; i++) {
      std::uniform_int_distribution<int> dist(i, n - 1);
      if (dist(rng) != i) {
        return false;
      }
    }
    return true;
  };

  std::print("Take cubes from the box in order)\n", n, n, n);
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(taked in order) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(taked not in order) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
