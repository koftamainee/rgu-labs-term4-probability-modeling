#include "quality.hpp"

Quality quality_from_int(int v) {
  if (v < QUALITY_MIN || v > QUALITY_MAX) {
    throw std::invalid_argument("invalid quality value");
  }
  return static_cast<Quality>(v);
}

QualityModuleSet::QualityModuleSet(int count) {
  set_count(count);
}

void QualityModuleSet::set_count(int count) {
  if (count < 0 || count > MAX_QUALITY_MODULES) {
    throw std::invalid_argument("module count must be in [0, 4]");
  }
  m_count = count;
  recompute();
}

int QualityModuleSet::count() const noexcept {
  return m_count;
}

double QualityModuleSet::speed_multiplier() const noexcept {
  return 1.0 + m_count * MODULE_SPEED_PENALTY;
}

const std::array<double, 4>& QualityModuleSet::upgrade_probs() const noexcept {
  return m_upgrade_probs;
}

double QualityModuleSet::total_upgrade_prob() const noexcept {
  double sum = 0.0;
  for (const double p : m_upgrade_probs) { sum += p; }
  return sum;
}

void QualityModuleSet::recompute() noexcept {
  for (int i = 1; i <= 4; i++) {
    m_upgrade_probs[i - 1] = m_count * single_module_upgrade_prob(i);
  }
}