#ifndef FACTORIOSIMULATION_RECYCLER_HPP
#define FACTORIOSIMULATION_RECYCLER_HPP

#include "inventory.hpp"
#include "quality.hpp"
#include "recipe.hpp"

#include <map>
#include <optional>
#include <random>
#include <vector>

struct RecyclerTask {
    Quality product_quality;
    int finish_tick;
};

class Recycler final {
public:
    explicit Recycler(int quality_modules = 0);

    bool is_busy() const noexcept;
    const QualityModuleSet& modules() const noexcept;

    bool try_start(Quality product_quality, int current_tick,
                   int base_recycle_time, Inventory& inventory);

    template <typename RNG>
    std::optional<Quality> try_complete(int current_tick,
                                        const Recipe& recipe,
                                        Inventory& inventory,
                                        RNG& rng) {
        if (!m_task.has_value()) { return std::nullopt; }
        if (current_tick < m_task->finish_tick) { return std::nullopt; }

        const Quality component_quality = m_modules.roll(m_task->product_quality, rng);
        const auto yield = compute_yield(recipe, rng);
        for (const auto& [type_id, count] : yield) {
            inventory.add_component(type_id, component_quality, count);
        }

        m_task.reset();
        return component_quality;
    }


private:
    QualityModuleSet m_modules;
    std::optional<RecyclerTask> m_task;

    int recycle_time(int base_recycle_time) const noexcept;

    template <typename RNG>
    static std::map<int, int> compute_yield(const Recipe& recipe, RNG& rng) {
        int total = 0;
        for (const auto& ing : recipe.ingredients()) { total += ing.count; }
        const int budget = static_cast<int>(std::floor(total * 0.25));
        if (budget == 0) { return {}; }

        std::vector<int> type_ids;
        type_ids.reserve(recipe.ingredients().size());
        for (const auto& ing : recipe.ingredients()) {
            type_ids.push_back(ing.type_id);
        }

        std::uniform_int_distribution<int> dist(0, static_cast<int>(type_ids.size()) - 1);
        std::map<int, int> result;
        for (int i = 0; i < budget; i++) {
            result[type_ids[dist(rng)]] += 1;
        }
        return result;
    }
};

#endif // FACTORIOSIMULATION_RECYCLER_HPP