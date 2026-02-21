#include <print>
#include <random>

#include "task_runner.hpp"

int main() {
  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::uniform_int_distribution<int> d(1, 6);
    int a = d(rng);
    int b = d(rng);
    int c = d(rng);
    return std::tuple{a, b, c};
  };

  auto results = runner.run(experiment, simulations);

  size_t count_all_five = 0;
  size_t count_all_equal = 0;

  for (const auto& [a, b, c] : results) {
    if (a == 5 && b == 5 && c == 5) count_all_five++;

    if (a == b && b == c) count_all_equal++;
  }

  std::println("P(all five) = {:.6f}",
               static_cast<double>(count_all_five) / simulations);

  std::println("P(all equal) = {:.6f}",
               static_cast<double>(count_all_equal) / simulations);
}
