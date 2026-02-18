#include <cstdlib>
#include <print>

#include "task_runner.hpp"

int main() {
  int total = 12;
  int high_grade = 8;
  int taken = 9;
  int target = 5;

  size_t simulations = 1e7;

  TaskRunner runner;

  auto experiment = [total, high_grade, taken, target](auto& rng) {
    std::vector<int> students(total, 0);
    std::fill(students.begin(), students.begin() + high_grade, 1);

    std::vector<int> hand(taken);
    std::sample(students.begin(), students.end(), hand.begin(), taken, rng);

    int count = std::count(hand.begin(), hand.end(), 1);

    return count == target;
  };

  std::print("Taking students from list\n");
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  std::print("P(5 with high grades from 8 taken) = {:.6f}\n",
             static_cast<double>(counts[true]) / simulations);
  std::print("P(not 5 with high grades from 8 taken) = {:.6f}\n",
             static_cast<double>(counts[false]) / simulations);
}
