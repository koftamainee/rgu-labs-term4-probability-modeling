#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;
  double a = 1.0;
  double x = 0.4;

  auto experiment = [a, x](auto& rng) {
    std::uniform_real_distribution<double> point(0.0, a);
    int near = 0;
    int far = 0;
    for (int i = 0; i < 5; i++) {
      double p = point(rng);
      if (p < x) { near++; }
      else { far++; }
    }
    return near == 2 && far == 3;
  };

  TaskRunner runner;
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("a = {:.2f}, x = {:.2f}\n", a, x);
  std::print("P(2 points closer than x, 3 farther) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
}