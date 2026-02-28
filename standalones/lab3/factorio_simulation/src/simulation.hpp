#ifndef FACTORIOSIMULATION_SIMULATION_HPP
#define FACTORIOSIMULATION_SIMULATION_HPP

#include "assembler.hpp"
#include "inventory.hpp"
#include "recipe.hpp"
#include "recycler.hpp"

#include <functional>
#include <map>
#include <random>
#include <vector>

struct SimulationConfig {
  int assembler_count;
  int recycler_count;
  int assembler_quality_modules;
  int recycler_quality_modules;
  int base_craft_time;
  int base_recycle_time;
  Recipe recipe;
  std::vector<int> supply_rates;
};

struct SimulationResult {
  std::vector<int> rare_ticks;
  std::vector<int> epic_ticks;
  std::vector<int> legendary_ticks;
};

using ProgressCallback = std::function<void(const SimulationResult&, int tick)>;

class Simulation final {
public:
  static constexpr int LEGENDARY_GOAL    = 25;
  static constexpr int PROGRESS_INTERVAL = 1000;

  explicit Simulation(SimulationConfig config,
                      ProgressCallback on_progress = nullptr,
                      unsigned int rng_seed = 42);

  SimulationResult run();

private:
  SimulationConfig  m_config;
  Inventory         m_inventory;
  std::vector<Assembler> m_assemblers;
  std::vector<Recycler>  m_recyclers;
  std::mt19937      m_rng;
  ProgressCallback  m_on_progress;

  void step_supply();
  void step_complete(int tick, SimulationResult& result);
  void step_schedule_assemblers(int tick);
  void step_schedule_recyclers(int tick);

  Quality best_available_quality() const noexcept;
  bool    should_recycle()         const noexcept;
};

#endif // FACTORIOSIMULATION_SIMULATION_HPP