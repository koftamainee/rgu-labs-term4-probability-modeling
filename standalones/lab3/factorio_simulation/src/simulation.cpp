#include "simulation.hpp"

Simulation::Simulation(SimulationConfig config,
                       ProgressCallback on_progress,
                       unsigned int rng_seed)
    : m_config(std::move(config))
    , m_rng(rng_seed)
    , m_on_progress(std::move(on_progress)) {
    for (int i = 0; i < m_config.assembler_count; i++) {
        m_assemblers.emplace_back(m_config.assembler_quality_modules);
    }
    for (int i = 0; i < m_config.recycler_count; i++) {
        m_recyclers.emplace_back(m_config.recycler_quality_modules);
    }
}

SimulationResult Simulation::run() {
    SimulationResult result;

    for (int tick = 0; ; tick++) {
        step_supply();
        step_complete(tick, result);

        if (static_cast<int>(result.legendary_ticks.size()) >= LEGENDARY_GOAL) {
            break;
        }

        step_schedule_assemblers(tick);
        step_schedule_recyclers(tick);

        if (m_on_progress && tick % PROGRESS_INTERVAL == 0) {
            m_on_progress(result, tick);
        }
    }

    return result;
}

void Simulation::step_supply() {
    const auto& ingredients = m_config.recipe.ingredients();
    for (int i = 0; i < static_cast<int>(ingredients.size()); i++) {
        const int rate = (i < static_cast<int>(m_config.supply_rates.size()))
                         ? m_config.supply_rates[i] : 0;
        if (rate > 0) {
            m_inventory.add_component(ingredients[i].type_id, Quality::Normal, rate);
        }
    }
}

void Simulation::step_complete(int tick, SimulationResult& result) {
    for (auto& assembler : m_assemblers) {
        const auto quality = assembler.try_complete(tick, m_inventory, m_rng);
        if (!quality.has_value()) { continue; }
        if (*quality == Quality::Rare)      { result.rare_ticks.push_back(tick); }
        if (*quality == Quality::Epic)      { result.epic_ticks.push_back(tick); }
        if (*quality == Quality::Legendary) { result.legendary_ticks.push_back(tick); }
    }

    for (auto& recycler : m_recyclers) {
        recycler.try_complete(tick, m_config.recipe, m_inventory, m_rng);
    }
}

void Simulation::step_schedule_assemblers(int tick) {
    const Quality q = best_available_quality();
    if (static_cast<int>(q) < QUALITY_MIN) {
        return;
    }

    for (auto& assembler : m_assemblers) {
        if (assembler.is_busy()) { continue; }
        assembler.try_start(m_config.recipe, q, tick, m_config.base_craft_time, m_inventory);
    }
}

void Simulation::step_schedule_recyclers(int tick) {
    if (!should_recycle()) { return; }

    for (auto& recycler : m_recyclers) {
        if (recycler.is_busy()) { continue; }
        for (int ql = static_cast<int>(Quality::Legendary);
             ql >= static_cast<int>(Quality::Normal); ql--) {
            const Quality q = quality_from_int(ql);
            if (q == Quality::Legendary) { continue; }
            if (recycler.try_start(q, tick, m_config.base_recycle_time, m_inventory)) {
                break;
            }
        }
    }
}

Quality Simulation::best_available_quality() const noexcept {
    for (int ql = QUALITY_MAX; ql >= QUALITY_MIN; ql--) {
        const Quality q = quality_from_int(ql);
        if (m_inventory.can_craft(m_config.recipe, q)) {
            return q;
        }
    }
    return static_cast<Quality>(QUALITY_MIN - 1);
}

bool Simulation::should_recycle() const noexcept {
    for (int ql = static_cast<int>(Quality::Legendary);
         ql >= static_cast<int>(Quality::Normal); ql--) {
        if (ql == static_cast<int>(Quality::Legendary)) { continue; }
        const Quality q = quality_from_int(ql);
        if (m_inventory.product_count(q) > 0) { return true; }
    }
    return false;
}