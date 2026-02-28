#include "assembler.hpp"

#include <cmath>
#include <iostream>

Assembler::Assembler(int quality_modules)
    : m_modules(quality_modules) {}

bool Assembler::is_busy() const noexcept {
  return m_task.has_value();
}

const QualityModuleSet& Assembler::modules() const noexcept {
  return m_modules;
}

bool Assembler::try_start(const Recipe& recipe, Quality input_quality,
                          int current_tick, int base_craft_time,
                          Inventory& inventory) {
  if (is_busy()) { return false; }
  if (!inventory.can_craft(recipe, input_quality)) { return false; }

  inventory.consume_for_craft(recipe, input_quality);
  m_task = AssemblerTask{
    input_quality,
    current_tick + craft_time(base_craft_time)
};
  return true;
}

int Assembler::craft_time(int base_craft_time) const noexcept {
  return static_cast<int>(std::ceil(base_craft_time * m_modules.speed_multiplier()));
}
