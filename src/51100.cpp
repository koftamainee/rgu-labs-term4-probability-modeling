#include <print>
#include "task_runner.hpp"

struct Result {
  bool error_detected;
  bool first_wrong;
};

int main() {
  TaskRunner runner;
  constexpr size_t N = 1'000'000;

  auto results = runner.run([](std::mt19937& rng) -> Result {
      std::bernoulli_distribution err1(0.05);
      std::bernoulli_distribution err2(0.1);
      bool e1 = err1(rng);
      bool e2 = err2(rng);
      bool detected = e1 != e2;
      return {detected, e1};
  }, N);

  size_t detected = 0, first_wrong = 0;
  for (const auto& r : results) {
    if (r.error_detected) {
      detected++;
      if (r.first_wrong) first_wrong++;
    }
  }

  std::println("P(B1|A) = {:.4f}", static_cast<double>(first_wrong) / detected);

  return 0;
}