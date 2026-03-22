#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;
  double p_fail = 0.1;

  auto exp_a = [p_fail](auto& rng) {
    std::bernoulli_distribution fail(p_fail);
    int working = 0;
    for (int i = 0; i < 3; i++) {
      if (!fail(rng)) { working++; }
    }
    return working == 3;
  };

  auto exp_b = [p_fail](auto& rng) {
    std::bernoulli_distribution fail(p_fail);
    int working = 0;
    for (int i = 0; i < 4; i++) {
      if (!fail(rng)) { working++; }
    }
    return working >= 3;
  };

  auto exp_c = [p_fail](auto& rng) {
    std::bernoulli_distribution fail(p_fail);
    int working = 0;
    for (int i = 0; i < 5; i++) {
      if (!fail(rng)) { working++; }
    }
    return working >= 3;
  };

  TaskRunner runner;

  auto r_a = runner.run(exp_a, simulations);
  std::println();
  auto r_b = runner.run(exp_b, simulations);
  std::println();
  auto r_c = runner.run(exp_c, simulations);
  std::println();

  auto c_a = tally(r_a);
  auto c_b = tally(r_b);
  auto c_c = tally(r_c);

  std::print("a) P(no failure, 3 main)           = {:.6f}\n",
             static_cast<double>(c_a[true]) / simulations);
  std::print("b) P(no failure, 3 main + 1 spare) = {:.6f}\n",
             static_cast<double>(c_b[true]) / simulations);
  std::print("c) P(no failure, 3 main + 2 spare) = {:.6f}\n",
             static_cast<double>(c_c[true]) / simulations);
}