#include <print>
#include "task_runner.hpp"

int main() {
  TaskRunner runner;
  constexpr size_t N = 1'000'000;

  auto results = runner.run([](std::mt19937& rng) -> bool {
    std::uniform_int_distribution<int> urn1(1, 10);
    bool white1 = urn1(rng) <= 4;

    int white2 = 4 + (white1 ? 1 : 0);
    std::uniform_int_distribution<int> urn2(1, 11);
    bool white2_drawn = urn2(rng) <= white2;

    int white3 = 4 + (white2_drawn ? 1 : 0);
    std::uniform_int_distribution<int> urn3(1, 12);
    return urn3(rng) <= white3;
  }, N);

  size_t white = 0;
  for (bool r : results)
    if (r) white++;

  std::println("P(A) = {:.4f}", static_cast<double>(white) / N);

  return 0;
}