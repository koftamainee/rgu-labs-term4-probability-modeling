#pragma once
#include <random>
#include <vector>

struct WalkResult {
  bool fell_off_cliff;
  int steps_taken;
  int final_position;
};

class DrunkWalkSimulation {
public:
    DrunkWalkSimulation(int cliff_pos, int cafe_pos, double p_forward);

    WalkResult run_single();

    WalkResult run_single_bounded();

    std::vector<WalkResult> run_batch(int n);
    std::vector<WalkResult> run_batch_bounded(int n);

    void reset_walk();
    int step_once();
    bool is_finished() const;
    bool fell_off_cliff() const;
    int current_position() const;
    int steps_taken() const;

    int cliff_pos;
    int cafe_pos;
    double p_forward;

private:
    std::mt19937 m_rng;
    std::uniform_real_distribution<double> m_dist;

    int m_position;
    int m_steps;
    bool m_finished;
    bool m_cliff;
};
