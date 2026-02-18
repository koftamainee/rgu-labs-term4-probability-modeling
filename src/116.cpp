#include <cstdlib>
#include <print>
#include <random>

#include "task_runner.hpp"

int main() {
  int n = 10;
  size_t simulations = 1e8;

  TaskRunner runner;

  auto experiment = [n](auto& rng) {
    std::uniform_int_distribution<int> dist(0, n - 1);
    int x = dist(rng);
    int y = dist(rng);
    int z = dist(rng);

    int painted = 0;
    if (x == 0 || x == n - 1) {
      painted++;
    }
    if (y == 0 || y == n - 1) {
      painted++;
    }
    if (z == 0 || z == n - 1) {
      painted++;
    }

    return painted;
  };

  std::print("Cube painting experiment ({}x{}x{} small cubes)\n", n, n, n);
  auto results = runner.run(experiment, simulations);
  std::println();

  auto counts = tally(results);

  for (int faces = 1; faces <= 3; faces++) {
    size_t count = counts[faces];
    double prob = double(count) / simulations;
    std::print("Probability of {} painted faces = {:.6f}\n", faces, prob);
  }
}
