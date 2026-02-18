#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  double a = 100.0;

  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [a](auto& rng) {
    std::uniform_real_distribution<double> dist(0, 2 * a);
    auto point = dist(rng);
    return point > 0 && point < a;
  };

  std::print("Tossing coin to the surface\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(not crossed any lines) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(crossed a line) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
