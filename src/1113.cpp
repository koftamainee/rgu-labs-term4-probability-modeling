#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  int total = 100;
  int taken = 10;
  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [total, taken](auto& rng) {
    std::vector<int> deck(total);
    std::iota(deck.begin(), deck.end(), 0);

    std::vector<int> hand(taken);
    std::sample(deck.begin(), deck.end(), hand.begin(), taken, rng);

    return std::find(hand.begin(), hand.end(), 0) != hand.end();
  };

  std::print("Take cards from the box)\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(found) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(not found) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
