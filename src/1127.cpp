#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  double total = 100;
  double valid = total / 3;

  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [total, valid](auto& rng) {
    std::uniform_real_distribution<double> dist(0, total);
    auto point = dist(rng);
    auto segment_len = std::min(total - point, point);
    return segment_len > valid;
  };

  std::print("Putting a dot on a line\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(segment bigger than L/3) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(segment smaller than L/3) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
