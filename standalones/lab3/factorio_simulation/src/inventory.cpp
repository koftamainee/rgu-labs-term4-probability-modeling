#include "inventory.hpp"

#include <algorithm>
#include <cmath>

void Inventory::add_component(int type_id, Quality quality, int amount) {
    if (type_id < 0) {
        throw std::invalid_argument("type_id must be non-negative");
    }
    if (amount <= 0) {
        throw std::invalid_argument("amount must be positive");
    }
    m_components[type_id][quality] += amount;
}

void Inventory::add_product(Quality quality, int amount) {
    if (amount <= 0) {
        throw std::invalid_argument("amount must be positive");
    }
    m_products[quality] += amount;
}

void Inventory::take_product(Quality quality) {
    const auto it = m_products.find(quality);
    if (it == m_products.end() || it->second <= 0) {
        throw std::runtime_error("no product of requested quality available");
    }
    it->second -= 1;
    if (it->second == 0) {
        m_products.erase(it);
    }
}

int Inventory::component_count(int type_id, Quality quality) const noexcept {
    const auto type_it = m_components.find(type_id);
    if (type_it == m_components.end()) { return 0; }
    const auto qual_it = type_it->second.find(quality);
    if (qual_it == type_it->second.end()) { return 0; }
    return qual_it->second;
}

int Inventory::product_count(Quality quality) const noexcept {
    const auto it = m_products.find(quality);
    if (it == m_products.end()) { return 0; }
    return it->second;
}

int Inventory::best_component_quality(int type_id) const noexcept {
    const auto type_it = m_components.find(type_id);
    if (type_it == m_components.end()) { return QUALITY_MIN - 1; }
    int best = QUALITY_MIN - 1;
    for (const auto& [quality, count] : type_it->second) {
        if (count > 0 && static_cast<int>(quality) > best) {
            best = static_cast<int>(quality);
        }
    }
    return best;
}

bool Inventory::can_craft(const Recipe& recipe, Quality quality) const noexcept {
    return std::ranges::all_of(recipe.ingredients(), [&](const auto& ing) {
        return component_count(ing.type_id, quality) >= ing.count;
    });
}

void Inventory::consume_for_craft(const Recipe& recipe, Quality quality) {
    if (!can_craft(recipe, quality)) {
        throw std::runtime_error("not enough components to craft");
    }
    for (const auto& ing : recipe.ingredients()) {
        m_components[ing.type_id][quality] -= ing.count;
        if (m_components[ing.type_id][quality] == 0) {
            m_components[ing.type_id].erase(quality);
        }
    }
}