#include <print>
#include <vector>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::vector<int> balls = {1, 2, 3, 4, 5};
    std::shuffle(balls.begin(), balls.end(), rng);

    int first = balls[0];
    int second = balls[1];
    int third = balls[2];

    return std::tuple{first, second, third};
  };

  auto results = runner.run(experiment, simulations);

  int count_a = 0;
  int count_b = 0;

  for (const auto& [b1, b2, b3] : results) {
    if (b1 == 1 && b2 == 4 && b3 == 5) {
      count_a++;
    }

    std::vector<int> drawn = {b1, b2, b3};
    std::sort(drawn.begin(), drawn.end());
    if (drawn[0] == 1 && drawn[1] == 4 && drawn[2] == 5) {
      count_b++;
    }
  }

  std::println("P(exactly 1,4,5 in order) = {:.6f}",
               static_cast<double>(count_a) / simulations);
  std::println("P(balls are 1,4,5 in any order) = {:.6f}",
               static_cast<double>(count_b) / simulations);
}
