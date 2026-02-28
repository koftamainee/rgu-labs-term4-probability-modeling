#include "recycler.hpp"

#include <cmath>

Recycler::Recycler(int quality_modules)
    : m_modules(quality_modules) {}

bool Recycler::is_busy() const noexcept {
  return m_task.has_value();
}

const QualityModuleSet& Recycler::modules() const noexcept {
  return m_modules;
}

bool Recycler::try_start(Quality product_quality, int current_tick,
                         int base_recycle_time, Inventory& inventory) {
  if (is_busy()) { return false; }
  if (inventory.product_count(product_quality) == 0) { return false; }

  inventory.take_product(product_quality);
  m_task = RecyclerTask{
    product_quality,
    current_tick + recycle_time(base_recycle_time)
};
  return true;
}

int Recycler::recycle_time(int base_recycle_time) const noexcept {
  return static_cast<int>(std::ceil(base_recycle_time * m_modules.speed_multiplier()));
}
