#include <print>
#include "task_runner.hpp"

struct Result {
  bool hit;
  bool scoped;
};

int main() {
  TaskRunner runner;
  constexpr size_t N = 1'000'000;

  auto results = runner.run([](std::mt19937& rng) -> Result {
      std::uniform_int_distribution<int> pick(1, 10);
      bool scoped = pick(rng) <= 4;
      std::bernoulli_distribution hit(scoped ? 0.95 : 0.8);
      return {hit(rng), scoped};
  }, N);

  size_t hit_scoped = 0, hit_noscope = 0, total_hits = 0;
  for (const auto& r : results) {
    if (r.hit) {
      total_hits++;
      if (r.scoped) hit_scoped++;
      else          hit_noscope++;
    }
  }

  std::println("P(B1|A) = {:.4f}", static_cast<double>(hit_scoped)  / total_hits);
  std::println("P(B2|A) = {:.4f}", static_cast<double>(hit_noscope) / total_hits);

  return 0;
}