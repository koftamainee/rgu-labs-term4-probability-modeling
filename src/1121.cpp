#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  int total = 5;
  int valid = 3;
  int taken = 2;

  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [total, valid, taken](auto& rng) {
    std::vector<int> deck(total, 0);
    std::fill(deck.begin(), deck.begin() + valid, 1);

    std::vector<int> hand(taken);
    std::sample(deck.begin(), deck.end(), hand.begin(), taken, rng);

    int count = std::count(hand.begin(), hand.end(), 1);

    return count;
  };

  std::print("Taking parts from the box\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(one part is colored) = {:.6f}\n",
             static_cast<double>(counts[1]) / simulations);

  std::print("P(two parts are colored) = {:.6f}\n",
             static_cast<double>(counts[2]) / simulations);

  std::print("P(at least one part is colored) = {:.6f}\n",
             static_cast<double>(counts[1] + counts[2]) / simulations);

  std::print("P(none is colored) = {:.6f}\n",
             static_cast<double>(counts[0]) / simulations);
}
