#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;

  auto exp_a = [](auto& rng) {
    std::bernoulli_distribution event(0.4);
    int count = 0;
    for (int i = 0; i < 4; i++) {
      if (event(rng)) { count++; }
    }
    return count >= 3;
  };

  auto exp_b = [](auto& rng) {
    std::bernoulli_distribution event(0.8);
    int count = 0;
    for (int i = 0; i < 5; i++) {
      if (event(rng)) { count++; }
    }
    return count >= 4;
  };

  TaskRunner runner;

  auto r_a = runner.run(exp_a, simulations);
  std::println();
  auto r_b = runner.run(exp_b, simulations);
  std::println();

  auto c_a = tally(r_a);
  auto c_b = tally(r_b);

  std::print("a) P(A >= 3 in 4 trials, p=0.4)     = {:.6f}\n",
             static_cast<double>(c_a[true]) / simulations);
  std::print("b) P(B: A >= 4 in 5 trials, p=0.8)  = {:.6f}\n",
             static_cast<double>(c_b[true]) / simulations);
}