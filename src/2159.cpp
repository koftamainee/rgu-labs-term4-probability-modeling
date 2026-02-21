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

  size_t count_a = 0;
  size_t count_b = 0;
  size_t count_c = 0;

  for (const auto& [a, b, c] : results) {
    if ((a == 1 && b == 1 && c != 1) || (a == 1 && c == 1 && b != 1) ||
        (b == 1 && c == 1 && a != 1)) {
      count_a++;
    }

    if ((a == b && b != c) || (a == c && c != b) || (b == c && b != a)) {
      count_b++;
    }

    if (a != b && a != c && b != c) {
      count_c++;
    }
  }

  std::println("P(a) = {:.6f}", static_cast<double>(count_a) / simulations);
  std::println("P(b) = {:.6f}", static_cast<double>(count_b) / simulations);
  std::println("P(c) = {:.6f}", static_cast<double>(count_c) / simulations);
}
