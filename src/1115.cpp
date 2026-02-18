#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  int total = 5;
  int broken = 2;
  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [total, broken](auto& rng) {
    std::vector<int> deck(total);
    std::iota(deck.begin(), deck.end(), 0);

    std::vector<int> hand(2);
    std::sample(deck.begin(), deck.end(), hand.begin(), 2, rng);
    return hand[0] >= broken && hand[1] >= broken;
  };

  std::print("Powering on device with 2 broken sensors\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(powers on) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(not powers off) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
