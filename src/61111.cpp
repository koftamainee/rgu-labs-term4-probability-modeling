#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;

  auto play = [](auto& rng, int games) {
    std::uniform_int_distribution<int> dist(0, 1);
    int wins = 0;
    for (int i = 0; i < games; ++i)
      if (dist(rng) == 1) ++wins;
    return wins;
  };

  auto exp_a1 = [&play](auto& rng) { return play(rng, 2) == 1; };
  auto exp_a2 = [&play](auto& rng) { return play(rng, 4) == 2; };
  auto exp_b1 = [&play](auto& rng) { return play(rng, 4) >= 2; };
  auto exp_b2 = [&play](auto& rng) { return play(rng, 5) >= 3; };

  TaskRunner runner;

  std::print("Case a)\n");
  auto r_a1 = runner.run(exp_a1, simulations);
  std::println();
  auto r_a2 = runner.run(exp_a2, simulations);
  std::println();

  std::print("Case b)\n");
  auto r_b1 = runner.run(exp_b1, simulations);
  std::println();
  auto r_b2 = runner.run(exp_b2, simulations);
  std::println();

  auto c_a1 = tally(r_a1);
  auto c_a2 = tally(r_a2);
  auto c_b1 = tally(r_b1);
  auto c_b2 = tally(r_b2);

  double p_a1 = static_cast<double>(c_a1[true]) / simulations;
  double p_a2 = static_cast<double>(c_a2[true]) / simulations;
  double p_b1 = static_cast<double>(c_b1[true]) / simulations;
  double p_b2 = static_cast<double>(c_b2[true]) / simulations;

  std::print("a) P(win 1 of 2)   = {:.6f}\n", p_a1);
  std::print("a) P(win 2 of 4)   = {:.6f}\n", p_a2);
  std::print("   {} is more likely\n\n",
             p_a1 > p_a2 ? "win 1 of 2" : "win 2 of 4");

  std::print("b) P(win >=2 of 4) = {:.6f}\n", p_b1);
  std::print("b) P(win >=3 of 5) = {:.6f}\n", p_b2);
  std::print("   {} is more likely\n",
             p_b1 > p_b2 ? "win >=2 of 4" : "win >=3 of 5");
}