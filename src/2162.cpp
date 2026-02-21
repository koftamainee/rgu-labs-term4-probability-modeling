#include <cmath>
#include <print>
#include <random>

#include "task_runner.hpp"

struct Point {
  double x;
  double y;
};

bool inside_triangle(const Point& p) {
  const double h = std::sqrt(3.0) / 2.0;
  if (p.y < -h / 3.0 || p.y > 2.0 * h / 3.0) {
    return false;
  }
  double left = -0.5 + (p.y + h / 3.0) / std::sqrt(3.0);
  double right = 0.5 - (p.y + h / 3.0) / std::sqrt(3.0);
  return p.x >= left && p.x <= right;
}

int segment_index(const Point& p) {
  double a1 = std::atan2(p.y - 2.0 * std::sqrt(3.0) / 3.0, p.x);
  double a2 = std::atan2(p.y + std::sqrt(3.0) / 3.0, p.x - 0.5);

  if (a1 > -M_PI / 3.0 && a1 < M_PI / 3.0) {
    return 1;
  }
  if (a2 > M_PI / 3.0 && a2 < M_PI) {
    return 2;
  }
  return 3;
}

int main() {
  const int simulations = 10000000;

  TaskRunner runner;

  auto experiment = [](auto& rng) {
    std::uniform_real_distribution<double> d(-1.0, 1.0);
    int count_triangle = 0;
    int count_seg1 = 0;
    int count_seg2 = 0;
    int count_seg3 = 0;

    for (int i = 0; i < 4; i++) {
      Point p;
      do {
        p.x = d(rng);
        p.y = d(rng);
      } while (p.x * p.x + p.y * p.y > 1.0);

      if (inside_triangle(p)) {
        count_triangle++;
      } else {
        int s = segment_index(p);
        if (s == 1) {
          count_seg1++;
        }
        if (s == 2) {
          count_seg2++;
        }
        if (s == 3) {
          count_seg3++;
        }
      }
    }

    bool a = (count_triangle == 4);
    bool b = (count_triangle == 1 && count_seg1 == 1 && count_seg2 == 1 &&
              count_seg3 == 1);

    return std::pair{a, b};
  };

  auto results = runner.run(experiment, simulations);

  int count_a = 0;
  int count_b = 0;

  for (int i = 0; i < simulations; i++) {
    if (results[i].first) {
      count_a++;
    }
    if (results[i].second) {
      count_b++;
    }
  }

  std::println("P(a) = {:.6f}", double(count_a) / simulations);
  std::println("P(b) = {:.6f}", double(count_b) / simulations);
}
