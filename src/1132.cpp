#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  double r = 100.0;
  double R = 200.0;

  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [r, R](auto& rng) {
    std::uniform_real_distribution<double> dist(0, R);
    auto point = dist(rng);
    return point > r;
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
