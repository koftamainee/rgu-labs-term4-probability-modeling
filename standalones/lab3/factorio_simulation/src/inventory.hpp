#ifndef FACTORIOSIMULATION_INVENTORY_HPP
#define FACTORIOSIMULATION_INVENTORY_HPP

#include "quality.hpp"
#include "recipe.hpp"

#include <map>
#include <stdexcept>

class Inventory final {
public:
  void add_component(int type_id, Quality quality, int amount);
  void add_product(Quality quality, int amount = 1);
  void take_product(Quality quality);

  int component_count(int type_id, Quality quality) const noexcept;
  int product_count(Quality quality) const noexcept;

  int best_component_quality(int type_id) const noexcept;

  bool can_craft(const Recipe& recipe, Quality quality) const noexcept;

  void consume_for_craft(const Recipe& recipe, Quality quality);

private:
  std::map<int, std::map<Quality, int>> m_components;

  std::map<Quality, int> m_products;
};

#endif // FACTORIOSIMULATION_INVENTORY_HPP