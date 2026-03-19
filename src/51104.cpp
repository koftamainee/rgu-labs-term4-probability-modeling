#include <print>

int main() {
  double pb1_a = 0.6;
  double pb2_a = 0.3;
  double pb3_a = 1.0 - pb1_a - pb2_a;

  std::println("P(B3|A) = {:.4f}", pb3_a);

  return 0;
}