#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;
  double p_boy = 0.51;
  int children = 5;

  auto count_boys = [p_boy, children](auto& rng) {
    std::bernoulli_distribution boy(p_boy);
    int boys = 0;
    for (int i = 0; i < children; i++) {
      if (boy(rng)) { boys++; }
    }
    return boys;
  };

  auto exp_a = [&count_boys](auto& rng) { return count_boys(rng) == 2; };
  auto exp_b = [&count_boys](auto& rng) { return count_boys(rng) <= 2; };
  auto exp_c = [&count_boys](auto& rng) { return count_boys(rng) > 2; };
  auto exp_d = [&count_boys](auto& rng) {
    int b = count_boys(rng);
    return b >= 2 && b <= 3;
  };

  TaskRunner runner;

  auto r_a = runner.run(exp_a, simulations);
  std::println();
  auto r_b = runner.run(exp_b, simulations);
  std::println();
  auto r_c = runner.run(exp_c, simulations);
  std::println();
  auto r_d = runner.run(exp_d, simulations);
  std::println();

  auto c_a = tally(r_a);
  auto c_b = tally(r_b);
  auto c_c = tally(r_c);
  auto c_d = tally(r_d);

  std::print("a) P(exactly 2 boys)      = {:.6f}\n",
             static_cast<double>(c_a[true]) / simulations);
  std::print("b) P(no more than 2 boys) = {:.6f}\n",
             static_cast<double>(c_b[true]) / simulations);
  std::print("c) P(more than 2 boys)    = {:.6f}\n",
             static_cast<double>(c_c[true]) / simulations);
  std::print("d) P(2 to 3 boys)         = {:.6f}\n",
             static_cast<double>(c_d[true]) / simulations);
}