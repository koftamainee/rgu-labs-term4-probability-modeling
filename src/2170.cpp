#include <print>
#include <random>
#include <vector>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  auto experiment_without_replacement = [](auto& rng) {
    std::vector<int> cubes = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    std::shuffle(cubes.begin(), cubes.end(), rng);

    int first = cubes[0];
    int second = cubes[1];
    int third = cubes[2];

    return std::tuple{first, second, third};
  };

  auto experiment_with_replacement = [](auto& rng) {
    std::uniform_int_distribution<int> dist(1, 10);

    int first = dist(rng);
    int second = dist(rng);
    int third = dist(rng);

    return std::tuple{first, second, third};
  };

  auto results_without =
      runner.run(experiment_without_replacement, simulations);
  auto results_with = runner.run(experiment_with_replacement, simulations);

  int count_without = 0;
  int count_with = 0;

  for (const auto& [c1, c2, c3] : results_without) {
    if (c1 == 1 && c2 == 2 && c3 == 3) {
      count_without++;
    }
  }

  for (const auto& [c1, c2, c3] : results_with) {
    if (c1 == 1 && c2 == 2 && c3 == 3) {
      count_with++;
    }
  }

  std::println("P(1,2,3 in order without replacement) = {:.6f}",
               static_cast<double>(count_without) / simulations);
  std::println("P(1,2,3 in order with replacement) = {:.6f}",
               static_cast<double>(count_with) / simulations);
}
