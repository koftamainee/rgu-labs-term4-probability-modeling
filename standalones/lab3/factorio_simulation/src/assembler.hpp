#ifndef FACTORIOSIMULATION_ASSEMBLER_HPP
#define FACTORIOSIMULATION_ASSEMBLER_HPP

#include <iostream>

#include "inventory.hpp"
#include "quality.hpp"
#include "recipe.hpp"

#include <optional>

struct AssemblerTask {
  Quality input_quality;
  int finish_tick;
};

class Assembler final {
public:
  explicit Assembler(int quality_modules = 0);

  bool is_busy() const noexcept;
  const QualityModuleSet& modules() const noexcept;

  bool try_start(const Recipe& recipe, Quality input_quality,
                 int current_tick, int base_craft_time,
                 Inventory& inventory);

  template <typename RNG>
  std::optional<Quality> try_complete(int current_tick,
                                      Inventory& inventory,
                                      RNG& rng) {
    if (!m_task.has_value()) {
      return std::nullopt;
    }
    if (current_tick < m_task->finish_tick) {
      return std::nullopt;
    }

    const Quality result = m_modules.roll(m_task->input_quality, rng);
    inventory.add_product(result);
    m_task.reset();
    return result;
  }

private:
  QualityModuleSet m_modules;
  std::optional<AssemblerTask> m_task;

  int craft_time(int base_craft_time) const noexcept;
};

#endif // FACTORIOSIMULATION_ASSEMBLER_HPP