#include <algorithm>
#include <cmath>
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

  auto results = runner.run([](std::mt19937& rng) -> SimResult {
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
    uint16_t K16 = key_dist(rng);

    std::uniform_int_distribution<uint16_t> msg(0, 65535);
    std::unordered_set<uint16_t> X1_msgs, X2_msgs;
    while (X1_msgs.size() < K) X1_msgs.insert(msg(rng));
    while (X2_msgs.size() < K) X2_msgs.insert(msg(rng));

    std::unordered_set<uint16_t> ciphertexts_1, ciphertexts_2;
    for (auto m : X1_msgs) ciphertexts_1.insert(m ^ K16);
    for (auto m : X2_msgs) ciphertexts_2.insert(m ^ K16);

    bool collision = false;
    for (auto c : ciphertexts_1)
      if (ciphertexts_2.count(c)) {
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
  double p_lower_bound = 1.0 - std::exp(
                             -static_cast<double>(K * K) / N_UNIVERSE);
  double p_exact = 1.0 - std::pow(1.0 - static_cast<double>(K) / N_UNIVERSE, K);
  double p_vigenere = static_cast<double>(collision_count) / SIM;
  double k_threshold = std::sqrt(N_UNIVERSE * std::log(2.0));

  std::println(
      "=== Birthday Problem / Vigenere Attack (N={}, k={}, sims={}) ===",
      N_UNIVERSE, K, SIM);
  std::println("");
  std::println("R(k,N) -- P(X and Y share at least one element):");
  std::println("  Empirical    : {:.6f}", p_empirical);
  std::println("  Exact        : {:.6f}  [1 - (1 - k/N)^k]", p_exact);
  std::println("  Lower bound  : {:.6f}  [1 - exp(-k^2/N)]", p_lower_bound);
  std::println("");
  std::println("R(k,N) > 1/2 when k > sqrt(N*ln2) ~= {:.1f}", k_threshold);
  std::println("  Current k={} is {} the threshold", K,
               K > static_cast<size_t>(k_threshold) ? "ABOVE" : "BELOW");
  std::println("");
  std::println("Vigenere collision P(E(x1)=E(x2), x1 in X1, x2 in X2):");
  std::println("  Empirical    : {:.6f}", p_vigenere);

  return 0;
}