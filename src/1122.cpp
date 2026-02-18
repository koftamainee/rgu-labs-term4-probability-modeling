#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  int total = 1000;

  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [total](auto& rng) {
    std::uniform_int_distribution<int> dist(0, total);
    return dist(rng) == 0;
  };

  std::print("Unlocking lock with 4 axis\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(right_combo) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(wrong_combo) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
