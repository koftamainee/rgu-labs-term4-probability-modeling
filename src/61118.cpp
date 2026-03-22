#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;

  auto experiment = [](auto& rng) {
    std::uniform_real_distribution<double> point(0.0, 4.0);
    int parts[4] = {0, 0, 0, 0};
    for (int i = 0; i < 8; i++) {
      double p = point(rng);
      int idx = static_cast<int>(p);
      if (idx == 4) { idx = 3; }
      ++parts[idx];
    }
    for (int i = 0; i < 4; i++) {
      if (parts[i] != 2) { return false; }
    }
    return true;
  };

  TaskRunner runner;
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(exactly 2 points in each of 4 parts) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
}