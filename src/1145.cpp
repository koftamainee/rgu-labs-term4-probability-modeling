
#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::uniform_real_distribution<double> dist(0, 1);
    auto x = dist(rng);
    auto y = dist(rng);
    return x + y < 1.0 && (x * y > 0.09 || std::abs(x - y) < 1e-9);
  };

  std::print("Taking x and y from [0,1]\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(x + y < 1.0 && xy >= 0.09) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(x + y >= 1.0 || xy < 0.09) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
