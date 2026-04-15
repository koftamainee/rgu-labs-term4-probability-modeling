#include "random_walk_simulation.hpp"
#include <algorithm>
RandomWalkSimulation::RandomWalkSimulation(const DiscreteRandomVariable& step_dist, double start_pos, int total_steps)
    : m_step_dist(step_dist), m_rng(std::random_device{}()), m_pos(start_pos), m_steps_taken(0), m_total_steps(total_steps), m_finished(false) {
  m_pmf = step_dist.pmf();
  std::sort(m_pmf.begin(), m_pmf.end());
}

bool RandomWalkSimulation::step() {
  if (m_finished) return false;
  double r = std::uniform_real_distribution<double>(0.0, 1.0)(m_rng);
  double cum = 0.0;
  double step_val = 0.0;
  for (const auto& [val, prob] : m_pmf) {
    cum += prob;
    if (r <= cum) {
      step_val = val;
      break;
    }
  }
  m_pos += step_val;
  m_steps_taken++;
  if (m_steps_taken >= m_total_steps) m_finished = true;
  return true;
}

double RandomWalkSimulation::current_position() const { return m_pos; }
int RandomWalkSimulation::steps_taken() const { return m_steps_taken; }
bool RandomWalkSimulation::is_finished() const { return m_finished; }
void RandomWalkSimulation::reset(double start_pos, int total_steps) {
  m_pos = start_pos;
  m_steps_taken = 0;
  m_total_steps = total_steps;
  m_finished = false;
}