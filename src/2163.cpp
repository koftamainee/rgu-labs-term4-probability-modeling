#include <print>
#include <random>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double a = dist(rng);
    double b = dist(rng);
    double c = dist(rng);
    return std::tuple{a, b, c};
  };

  auto results = runner.run(experiment, simulations);

  int count = 0;

  for (const auto& [x1, x2, x3] : results) {
    int part1 = 0;
    int part2 = 0;
    int part3 = 0;

    if (x1 < 1.0 / 3.0) {
      part1++;
    } else if (x1 < 2.0 / 3.0) {
      part2++;
    } else {
      part3++;
    }

    if (x2 < 1.0 / 3.0) {
      part1++;
    } else if (x2 < 2.0 / 3.0) {
      part2++;
    } else {
      part3++;
    }

    if (x3 < 1.0 / 3.0) {
      part1++;
    } else if (x3 < 2.0 / 3.0) {
      part2++;
    } else {
      part3++;
    }

    if (part1 == 1 && part2 == 1 && part3 == 1) {
      count++;
    }
  }

  std::println("P(each part gets one point) = {:.6f}",
               static_cast<double>(count) / simulations);
}
