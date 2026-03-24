#include <print>
#include "task_runner.hpp"

int main() {
  size_t simulations = 1e7;

  auto lamp_failure = [](auto& rng, double p) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng) < p;
  };

  auto exactly_two_fail = [&lamp_failure](auto& rng) {
    double p1 = 0.1, p2 = 0.2, p3 = 0.3, p4 = 0.4;
    bool f1 = lamp_failure(rng, p1);
    bool f2 = lamp_failure(rng, p2);
    bool f3 = lamp_failure(rng, p3);
    bool f4 = lamp_failure(rng, p4);
    int fail_count = (f1 ? 1 : 0) + (f2 ? 1 : 0) + (f3 ? 1 : 0) + (f4 ? 1 : 0);
    return fail_count == 2;
  };

  auto first_two_failed = [&lamp_failure](auto& rng) {
    double p1 = 0.1, p2 = 0.2, p3 = 0.3, p4 = 0.4;
    bool f1 = lamp_failure(rng, p1);
    bool f2 = lamp_failure(rng, p2);
    bool f3 = lamp_failure(rng, p3);
    bool f4 = lamp_failure(rng, p4);
    return f1 && f2 && !f3 && !f4;
  };

  TaskRunner runner;

  auto cond_count = runner.run(exactly_two_fail, simulations);
  auto event_count = runner.run(first_two_failed, simulations);

  auto cond_tally = tally(cond_count);
  auto event_tally = tally(event_count);

  double p_exactly_two = static_cast<double>(cond_tally[true]) / simulations;
  double p_first_two_and_exactly_two = static_cast<double>(event_tally[true]) / simulations;

  double result = p_first_two_and_exactly_two / p_exactly_two;

  std::print("P(exactly two lamps fail) = {:.6f}\n", p_exactly_two);
  std::print("P(first and second fail and exactly two fail) = {:.6f}\n", p_first_two_and_exactly_two);
  std::print("P(first and second fail | exactly two fail) = {:.6f}\n", result);
}