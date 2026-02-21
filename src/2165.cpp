#include <print>
#include <vector>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::vector<int> tickets(100);
    for (int i = 0; i < 100; i++) {
      tickets[i] = (i < 5) ? 1 : 0;
    }

    std::shuffle(tickets.begin(), tickets.end(), rng);

    int first = tickets[0];
    int second = tickets[1];

    return std::tuple{first, second};
  };

  auto results = runner.run(experiment, simulations);

  int count = 0;

  for (const auto& [t1, t2] : results) {
    if (t1 == 1 && t2 == 1) {
      count++;
    }
  }

  std::println("P(both tickets are winning) = {:.6f}",
               static_cast<double>(count) / simulations);
}
