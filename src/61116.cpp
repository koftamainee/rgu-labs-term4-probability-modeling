#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;

  auto experiment = [](auto& rng) {
    std::uniform_real_distribution<double> point(0.0, 3.0);
    int left = 0;
    int right = 0;
    for (int i = 0; i < 4; i++) {
      double x = point(rng);
      if (x < 2.0) { left++; } else { right++; }
    }
    return left == 2 && right == 2;
  };

  TaskRunner runner;
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(2 points left of C, 2 right of C) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
}