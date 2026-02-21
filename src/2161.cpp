#include <print>
#include <random>

#include "task_runner.hpp"

int main() {
  const double p_hit = 0.8;
  const double limit = 0.4;
  const size_t simulations = 1e7;

  TaskRunner runner;

  std::bernoulli_distribution hit(p_hit);

  int n = 1;
  for (;; n++) {
    auto experiment = [&](auto& rng) {
      for (int i = 0; i < n; i++) {
        if (!hit(rng)) {
          return false;
        }
      }
      return true;
    };

    auto results = runner.run(experiment, simulations);

    size_t success = 0;
    for (bool r : results) {
      if (r) {
        success++;
      }
    }

    double p_all_hit = static_cast<double>(success) / simulations;

    if (p_all_hit < limit) {
      std::println("n = {}", n);
      break;
    }
  }
}
