#ifndef FACTORIOSIMULATION_RECIPE_HPP
#define FACTORIOSIMULATION_RECIPE_HPP

#include <string>
#include <vector>

struct Ingredient {
  int         type_id;
  int         count;
  std::string name;
};

class Recipe final {
public:
  explicit Recipe(std::vector<Ingredient> ingredients,
                  std::string product_name = "product");

  const std::vector<Ingredient>& ingredients()  const noexcept;
  const std::string&             product_name() const noexcept;
  int                            component_count() const noexcept;

private:
  std::vector<Ingredient> m_ingredients;
  std::string             m_product_name;
};

#endif