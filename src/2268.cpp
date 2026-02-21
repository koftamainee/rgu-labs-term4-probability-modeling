#include <print>
#include <random>
#include <vector>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  double p_head = 0.3;
  double p_student = 0.5;

  for (int n = 0; n <= 5; n++) {
    auto experiment = [p_head, p_student, n](auto& rng) {
      std::uniform_real_distribution<double> dist(0.0, 1.0);

      bool informed = false;

      if (dist(rng) < p_head) {
        informed = true;
      }

      for (int i = 0; i < n; i++) {
        if (dist(rng) < p_student) {
          informed = true;
        }
      }

      return informed;
    };

    auto results = runner.run(experiment, simulations);

    int count = 0;
    for (bool informed : results) {
      if (informed) {
        count++;
      }
    }

    double probability = static_cast<double>(count) / simulations;

    if (probability > 0.9) {
      std::println("--> Need n = {} students besides the head", n);
      break;
    }
  }
}
