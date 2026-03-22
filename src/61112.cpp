#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;
  int tosses = 5;

  auto exp_a = [tosses](auto& rng) {
    std::bernoulli_distribution coin(0.5);
    int tails = 0;
    for (int i = 0; i < tosses; i++) {
      if (coin(rng)) { tails++; }
    }
    return tails < 2;
  };

  auto exp_b = [tosses](auto& rng) {
    std::bernoulli_distribution coin(0.5);
    int tails = 0;
    for (int i = 0; i < tosses; i++) {
      if (coin(rng)) { tails++; }
    }
    return tails >= 2;
  };

  TaskRunner runner;

  auto r_a = runner.run(exp_a, simulations);
  std::println();
  auto r_b = runner.run(exp_b, simulations);
  std::println();

  auto c_a = tally(r_a);
  auto c_b = tally(r_b);

  std::print("a) P(tails < 2)  = {:.6f}\n",
             static_cast<double>(c_a[true]) / simulations);
  std::print("b) P(tails >= 2) = {:.6f}\n",
             static_cast<double>(c_b[true]) / simulations);
}