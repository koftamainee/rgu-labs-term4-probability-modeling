#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  int total = 15;
  int lvov = 10;
  int taken = 5;
  int target = 3;

  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [total, lvov, taken, target](auto& rng) {
    std::vector<int> kinescopes(total, 0);
    std::fill(kinescopes.begin(), kinescopes.begin() + lvov, 1);

    std::vector<int> hand(taken);
    std::sample(kinescopes.begin(), kinescopes.end(), hand.begin(), taken, rng);

    int count = std::count(hand.begin(), hand.end(), 1);

    return count == target;
  };

  std::print("Taking telescopes from storage\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(3 from Lvov from 5 taken) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(not 3 from Lvov from 5 taken) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
