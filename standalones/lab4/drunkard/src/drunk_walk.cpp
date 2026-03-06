#include "drunk_walk.hpp"

DrunkWalkSimulation::DrunkWalkSimulation(int cliff_pos, int cafe_pos, double p_forward)
    : cliff_pos(cliff_pos),
      cafe_pos(cafe_pos),
      p_forward(p_forward),
      m_rng(std::random_device{}()),
      m_dist(0.0, 1.0),
      m_position(cafe_pos),
      m_steps(0),
      m_finished(false),
      m_cliff(false) {}

WalkResult DrunkWalkSimulation::run_single() {
    int pos = cafe_pos;
    int steps = 0;

    while (true) {
        pos += (m_dist(m_rng) < p_forward) ? 1 : -1;
        steps++;
        if (pos <= cliff_pos)
            return {true, steps, pos};
        if (pos >= cafe_pos)
            return {false, steps, pos};
    }
}

WalkResult DrunkWalkSimulation::run_single_bounded() {
    return run_single();
}

std::vector<WalkResult> DrunkWalkSimulation::run_batch(int n) {
    std::vector<WalkResult> results;
    results.reserve(n);
    for (int i = 0; i < n; i++)
        results.push_back(run_single());
    return results;
}

std::vector<WalkResult> DrunkWalkSimulation::run_batch_bounded(int n) {
    return run_batch(n);
}

void DrunkWalkSimulation::reset_walk() {
    m_position = cafe_pos;
    m_steps = 0;
    m_finished = false;
    m_cliff = false;
}

int DrunkWalkSimulation::step_once() {
    if (m_finished) return m_position;
    m_position += (m_dist(m_rng) < p_forward) ? 1 : -1;
    m_steps++;
    if (m_position <= cliff_pos) {
        m_finished = true;
        m_cliff = true;
    } else if (m_position >= cafe_pos) {
        m_finished = true;
        m_cliff = false;
    }
    return m_position;
}

bool DrunkWalkSimulation::is_finished()      const { return m_finished; }
bool DrunkWalkSimulation::fell_off_cliff()   const { return m_cliff; }
int  DrunkWalkSimulation::current_position() const { return m_position; }
int  DrunkWalkSimulation::steps_taken()      const { return m_steps; }
