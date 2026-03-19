#include <print>
#include "task_runner.hpp"

int main() {
  TaskRunner runner;
  constexpr size_t N = 1'000'000;

  auto results = runner.run([](std::mt19937& rng) -> bool {
    std::uniform_int_distribution<int> pick(1, 50);
    int d = pick(rng);

    double p_quality;
    if (d <= 12) p_quality = 0.9;
    else if (d <= 32) p_quality = 0.6;
    else p_quality = 0.9;

    std::bernoulli_distribution quality(p_quality);
    return quality(rng);
  }, N);

  size_t good = 0;
  for (bool r : results)
    if (r) good++;

  std::println("P(A) = {:.4f}", static_cast<double>(good) / N);

  return 0;
}