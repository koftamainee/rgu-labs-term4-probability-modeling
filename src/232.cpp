#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <numeric>

#include "task_runner.hpp"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <K>\n";
    return 1;
  }

  const size_t K = std::stoull(argv[1]);
  const int n = 10;

  TaskRunner runner;

  auto experiment = [](std::mt19937& rng) {
    std::vector<int> keys(n);
    std::iota(keys.begin(), keys.end(), 0);

    std::shuffle(keys.begin(), keys.end(), rng);

    for (int i = 0; i < n; i++) {
      if (keys[i] == 0) {
        return i + 1;
      }
    }
    return n;
  };

  auto results = runner.run(experiment, K);
  auto counts = tally(results);

  const double p_theory = 1.0 / n;

  std::cout << "\nResults\n";
  std::cout << "n = " << n << '\n';
  std::cout << "K = " << K << "\n\n";

  std::cout << "Step |" << std::setw(13) << "Count |"
            << " Probability\n";

  std::cout << "-----+-------------+------------\n";

  for (size_t i = 1; i <= n; ++i) {
    size_t count = counts[i];
    double p = static_cast<double>(count) / K;

    std::cout << std::setw(4) << i << " |" << std::setw(12) << count << " |"
              << std::fixed << std::setprecision(6) << std::setw(11) << p
              << '\n';
  }

  std::cout << "\nTheoretical probability: 1 / n = " << std::fixed
            << std::setprecision(6) << p_theory << '\n';

  return 0;
}
