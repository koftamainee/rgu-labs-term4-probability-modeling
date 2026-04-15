#pragma once
#include "discrete_random_variable.h"
#include <random>
#include <vector>

class RandomWalkSimulation {
public:
  RandomWalkSimulation(const DiscreteRandomVariable& step_dist, double start_pos, int total_steps);
  bool step();
  double current_position() const;
  int steps_taken() const;
  bool is_finished() const;
  void reset(double start_pos, int total_steps);

private:
  DiscreteRandomVariable m_step_dist;
  std::mt19937 m_rng;
  std::vector<std::pair<double, double>> m_pmf;
  double m_pos;
  int m_steps_taken;
  int m_total_steps;
  bool m_finished;
};