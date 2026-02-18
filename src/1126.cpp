#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  double total = 20;
  double valid = 10;

  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [total, valid](auto& rng) {
    std::uniform_real_distribution<double> dist(0, total);
    return dist(rng) < valid;
  };

  std::print("Putting a dot on a line\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(in the smaller segment) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(not in the smaller segment) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
