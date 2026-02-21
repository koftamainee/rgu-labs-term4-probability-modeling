#include <print>
#include <vector>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::vector<int> details(10);
    for (int i = 0; i < 10; i++) {
      details[i] = (i < 6) ? 1 : 0;
    }

    std::shuffle(details.begin(), details.end(), rng);

    int first = details[0];
    int second = details[1];
    int third = details[2];
    int fourth = details[3];

    return std::tuple{first, second, third, fourth};
  };

  auto results = runner.run(experiment, simulations);

  int count = 0;

  for (const auto& [d1, d2, d3, d4] : results) {
    if (d1 == 1 && d2 == 1 && d3 == 1 && d4 == 1) {
      count++;
    }
  }

  std::println("P(all four details are painted) = {:.6f}",
               static_cast<double>(count) / simulations);
}
