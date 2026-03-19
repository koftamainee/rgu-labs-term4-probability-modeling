#include <print>
#include <unordered_set>
#include "task_runner.hpp"

static constexpr size_t N_UNIVERSE = 65536;
static constexpr size_t K = 300;

struct SimResult {
  bool intersection_found;
  bool vigenere_collision;
};

int main() {
  TaskRunner runner;
  constexpr size_t SIM = 100'000;

  const auto results = runner.run([](std::mt19937& rng) -> SimResult {
    std::uniform_int_distribution<size_t> pick(0, N_UNIVERSE - 1);

    std::unordered_set<size_t> X, Y;
    while (X.size() < K) X.insert(pick(rng));
    while (Y.size() < K) Y.insert(pick(rng));

    bool intersect = false;
    for (auto v : X)
      if (Y.count(v)) {
        intersect = true;
        break;
      }

    std::uniform_int_distribution<uint16_t> key_dist(0, 65535);
    uint16_t key = key_dist(rng);

    std::uniform_int_distribution<uint16_t> msg(0, 65535);
    std::unordered_set<uint16_t> X1_msgs, X2_msgs;
    while (X1_msgs.size() < K) X1_msgs.insert(msg(rng));
    while (X2_msgs.size() < K) X2_msgs.insert(msg(rng));

    std::unordered_set<uint16_t> enc1, enc2;
    for (auto m : X1_msgs) enc1.insert(m ^ key);
    for (auto m : X2_msgs) enc2.insert(m ^ key);

    bool collision = false;
    for (auto c : enc1)
      if (enc2.contains(c)) {
        collision = true;
        break;
      }

    return {intersect, collision};
  }, SIM);

  size_t intersect_count = 0;
  size_t collision_count = 0;
  for (const auto& r : results) {
    if (r.intersection_found) intersect_count++;
    if (r.vigenere_collision) collision_count++;
  }

  double p_empirical = static_cast<double>(intersect_count) / SIM;
  double p_vernam = static_cast<double>(collision_count) / SIM;

  std::println("=== Birthday Problem / Vernam Attack (N={}, k={}, sims={}) ===",
               N_UNIVERSE, K, SIM);
  std::println("");
  std::println("R(k,N) -- P(X and Y share at least one element):");
  std::println("  Empirical : {:.6f}", p_empirical);
  std::println("");
  std::println("Vernam collision P(E(x1)=E(x2), x1 in X1, x2 in X2):");
  std::println("  Empirical : {:.6f}", p_vernam);

  return 0;
}