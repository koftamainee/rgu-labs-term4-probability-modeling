#include <print>
#include "task_runner.hpp"

int main() {
  TaskRunner runner;
  constexpr size_t N = 1'000'000;

  auto results = runner.run([](std::mt19937& rng) -> bool {
    std::uniform_int_distribution<int> rifle(1, 5);
    int r = rifle(rng);
    bool has_scope = r <= 3;

    std::bernoulli_distribution hit(has_scope ? 0.95 : 0.7);
    return hit(rng);
  }, N);

  size_t hits = 0;
  for (bool r : results)
    if (r) hits++;

  std::println("P(A) empirical  = {:.4f}", static_cast<double>(hits) / N);
  std::println("P(A) theoretical = 0.8500");

  return 0;
}