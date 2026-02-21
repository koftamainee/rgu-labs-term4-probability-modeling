#include <print>
#include <vector>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::vector<int> questions(25);
    for (int i = 0; i < 25; i++) {
      questions[i] = (i < 20) ? 1 : 0;
    }

    std::shuffle(questions.begin(), questions.end(), rng);

    int first = questions[0];
    int second = questions[1];
    int third = questions[2];

    return std::tuple{first, second, third};
  };

  auto results = runner.run(experiment, simulations);

  int count = 0;

  for (const auto& [q1, q2, q3] : results) {
    if (q1 == 1 && q2 == 1 && q3 == 1) {
      count++;
    }
  }

  std::println("P(student knows all three questions) = {:.6f}",
               static_cast<double>(count) / simulations);
}
