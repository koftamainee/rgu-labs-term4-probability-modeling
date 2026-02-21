#include <algorithm>
#include <map>
#include <print>
#include <random>
#include <vector>

#include "task_runner.hpp"

int main() {
  int simulations = 1e7;

  TaskRunner runner;

  std::vector<int> deck_sizes = {36, 52};

  for (int deck_size : deck_sizes) {
    int values_count = deck_size / 4;

    std::println("\n=== Deck size: {} cards ({} values) ===", deck_size,
                 values_count);

    auto experiment_without_replacement = [deck_size](auto& rng) {
      std::vector<int> deck(deck_size);
      for (int i = 0; i < deck_size; i++) {
        deck[i] = i / 4;
      }

      std::shuffle(deck.begin(), deck.end(), rng);

      int c1 = deck[0];
      int c2 = deck[1];
      int c3 = deck[2];
      int c4 = deck[3];

      return std::tuple{c1, c2, c3, c4};
    };

    auto experiment_with_replacement = [values_count](auto& rng) {
      std::uniform_int_distribution<int> dist(0, values_count - 1);

      int c1 = dist(rng);
      int c2 = dist(rng);
      int c3 = dist(rng);
      int c4 = dist(rng);

      return std::tuple{c1, c2, c3, c4};
    };

    auto results_without =
        runner.run(experiment_without_replacement, simulations);

    int count_3_1_without = 0;
    int count_2_2_without = 0;
    int count_all_diff_without = 0;
    int count_all_same_without = 0;

    for (const auto& [v1, v2, v3, v4] : results_without) {
      std::map<int, int> counts;
      counts[v1]++;
      counts[v2]++;
      counts[v3]++;
      counts[v4]++;

      std::vector<int> freq;
      for (const auto& [value, cnt] : counts) {
        freq.push_back(cnt);
      }
      std::sort(freq.begin(), freq.end());

      if (freq.size() == 2 && freq[0] == 1 && freq[1] == 3) {
        count_3_1_without++;
      } else if (freq.size() == 2 && freq[0] == 2 && freq[1] == 2) {
        count_2_2_without++;
      } else if (freq.size() == 4) {
        count_all_diff_without++;
      } else if (freq.size() == 1) {
        count_all_same_without++;
      }
    }

    std::println("Without replacement:");
    std::println("  P(3 cards of one value, 1 of another) = {:.6f}",
                 static_cast<double>(count_3_1_without) / simulations);
    std::println("  P(2 cards of one value, 2 of another) = {:.6f}",
                 static_cast<double>(count_2_2_without) / simulations);
    std::println("  P(all different values) = {:.6f}",
                 static_cast<double>(count_all_diff_without) / simulations);
    std::println("  P(all same value) = {:.6f}",
                 static_cast<double>(count_all_same_without) / simulations);

    auto results_with = runner.run(experiment_with_replacement, simulations);

    int count_3_1_with = 0;
    int count_2_2_with = 0;
    int count_all_diff_with = 0;
    int count_all_same_with = 0;

    for (const auto& [v1, v2, v3, v4] : results_with) {
      std::map<int, int> counts;
      counts[v1]++;
      counts[v2]++;
      counts[v3]++;
      counts[v4]++;

      std::vector<int> freq;
      for (const auto& [value, cnt] : counts) {
        freq.push_back(cnt);
      }
      std::sort(freq.begin(), freq.end());

      if (freq.size() == 2 && freq[0] == 1 && freq[1] == 3) {
        count_3_1_with++;
      } else if (freq.size() == 2 && freq[0] == 2 && freq[1] == 2) {
        count_2_2_with++;
      } else if (freq.size() == 4) {
        count_all_diff_with++;
      } else if (freq.size() == 1) {
        count_all_same_with++;
      }
    }

    std::println("With replacement:");
    std::println("  P(3 cards of one value, 1 of another) = {:.6f}",
                 static_cast<double>(count_3_1_with) / simulations);
    std::println("  P(2 cards of one value, 2 of another) = {:.6f}",
                 static_cast<double>(count_2_2_with) / simulations);
    std::println("  P(all different values) = {:.6f}",
                 static_cast<double>(count_all_diff_with) / simulations);
    std::println("  P(all same value) = {:.6f}",
                 static_cast<double>(count_all_same_with) / simulations);
  }
}
