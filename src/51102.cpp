#include <print>
#include "task_runner.hpp"

struct Result {
  bool approved;
  bool second;
};

int main() {
  TaskRunner runner;
  constexpr size_t N = 1'000'000;

  auto results = runner.run([](std::mt19937& rng) -> Result {
      std::bernoulli_distribution who(0.55);
      bool first = who(rng);
      std::bernoulli_distribution approve(first ? 0.9 : 0.98);
      return {approve(rng), !first};
  }, N);

  size_t approved = 0, second = 0;
  for (const auto& r : results) {
    if (r.approved) {
      approved++;
      if (r.second) second++;
    }
  }

  std::println("P(B2|A) = {:.4f}", static_cast<double>(second) / approved);

  return 0;
}