#include "recipe.hpp"
#include <stdexcept>

Recipe::Recipe(std::vector<Ingredient> ingredients, std::string product_name) {
  if (ingredients.empty())
    throw std::invalid_argument("recipe must have at least one ingredient");
  for (const auto& ing : ingredients) {
    if (ing.type_id < 0)
      throw std::invalid_argument("ingredient type_id must be non-negative");
    if (ing.count <= 0)
      throw std::invalid_argument("ingredient count must be positive");
  }
  m_ingredients  = std::move(ingredients);
  m_product_name = std::move(product_name);
}

const std::vector<Ingredient>& Recipe::ingredients() const noexcept {
  return m_ingredients;
}

const std::string& Recipe::product_name() const noexcept {
  return m_product_name;
}

int Recipe::component_count() const noexcept {
  return static_cast<int>(m_ingredients.size());
}