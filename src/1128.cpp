#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  double bigger_r = 200.0;
  double smaller_r = 75.0;

  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [bigger_r, smaller_r](auto& rng) {
    std::uniform_real_distribution<double> dist(0, bigger_r);
    auto point_distance = dist(rng);
    return point_distance < smaller_r ||
           std::abs(smaller_r - point_distance) < 1e-9;
  };

  std::print("Putting a dot in a circle\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(in the smaller circle) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(not in the smaller circle) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
