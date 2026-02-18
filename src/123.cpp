#include <cstdlib>
#include <print>
#include <random>

#include "task_runner.hpp"

struct Outcome {
  size_t tosses;
};

int main(int argc, char** argv) {
  if (argc < 2) {
    std::print("Usage: {} <k>\n", argv[0]);
    return 1;
  }

  size_t k = std::stoul(argv[1]);
  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::uniform_int_distribution<int> coin(0, 1);
    int last = -1;
    size_t tosses = 0;

    while (true) {
      int curr = coin(rng);
      tosses++;
      if (last != -1 && curr == last) {
        break;
      }
      last = curr;
    }

    return Outcome{tosses};
  };

  std::print("Coin toss experiment (stop after 2 consecutive same sides)\n");
  auto results = runner.run(experiment, simulations);

  size_t before_k = 0;
  size_t even_tosses = 0;

  for (const auto& r : results) {
    if (r.tosses < k) {
      before_k++;
    }
    if (r.tosses % 2 == 0) {
      even_tosses++;
    }
  }

  std::println();

  double p_before_k = double(before_k) / simulations;
  double p_even = double(even_tosses) / simulations;

  std::print("Parameter k = {}\n", k);
  std::print("P(experiment finishes before k tosses) = {:.6f}\n", p_before_k);
  std::print("P(experiment finishes with even number of tosses) = {:.6f}\n",
             p_even);
}
