#include <print>
#include "task_runner.hpp"

struct Result {
  bool exactly_two;
  bool third_hit;
};

int main() {
  TaskRunner runner;
  constexpr size_t N = 1'000'000;

  auto results = runner.run([](std::mt19937& rng) -> Result {
    std::bernoulli_distribution s1(0.6), s2(0.5), s3(0.4);
    bool h1 = s1(rng), h2 = s2(rng), h3 = s3(rng);
    int total = h1 + h2 + h3;
    return {total == 2, h3};
  }, N);

  size_t two_hits = 0, third_hit = 0;
  for (const auto& r : results) {
    if (r.exactly_two) {
      two_hits++;
      if (r.third_hit) third_hit++;
    }
  }

  std::println("P(B1|A) = {:.4f}", static_cast<double>(third_hit) / two_hits);

  return 0;
}