#include "simulation.hpp"
#include "reporter.hpp"
#include "config_parser.hpp"

#include <iostream>
#include <string>

static void print_progress(const SimulationResult& result, int tick) {
    const int legendary = static_cast<int>(result.legendary_ticks.size());
    const int epic       = static_cast<int>(result.epic_ticks.size());
    const int rare       = static_cast<int>(result.rare_ticks.size());
    constexpr int goal   = Simulation::LEGENDARY_GOAL;
    constexpr int BAR    = 30;
    const int filled     = (legendary * BAR) / goal;

    std::cerr << "\r" << "tick=" << tick
              << "  [" << std::string(filled, '#') << std::string(BAR - filled, '.') << "] "
              << legendary << "/" << goal << " legendary"
              << "  epic=" << epic << "  rare=" << rare << "   ";
    std::cerr.flush();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <config.cfg>\n";
        return 1;
    }

    try {
        const FactoryConfig cfg = parse_config(argv[1]);
        const SimulationConfig& sim_cfg = cfg.sim;
        const ReportConfig&     rep_cfg = cfg.report;

        std::cerr << "=== Factorio Simulation ===\n"
                  << "Product    : " << sim_cfg.recipe.product_name() << "\n"
                  << "Assemblers : " << sim_cfg.assembler_count
                  << " (modules: " << sim_cfg.assembler_quality_modules << ")\n"
                  << "Recyclers  : " << sim_cfg.recycler_count
                  << " (modules: " << sim_cfg.recycler_quality_modules << ")\n"
                  << "Craft time : " << sim_cfg.base_craft_time << " ticks\n"
                  << "Recycle    : " << sim_cfg.base_recycle_time << " ticks\n"
                  << "Ingredients:\n";
        for (int i = 0; i < static_cast<int>(sim_cfg.recipe.ingredients().size()); ++i) {
            const auto& ing = sim_cfg.recipe.ingredients()[i];
            std::cerr << "  " << ing.name << " x" << ing.count
                      << "  (supply: " << sim_cfg.supply_rates[i] << "/tick)\n";
        }
        std::cerr << "Goal       : " << Simulation::LEGENDARY_GOAL << " Legendary\n"
                  << "===========================\n\n";

        Simulation sim{sim_cfg, print_progress};
        const SimulationResult result = sim.run();
        std::cerr << "\n\n";

#ifdef PDFLATEX_AVAILABLE
        constexpr bool compile = true;
#else
        constexpr bool compile = false;
#endif
        Reporter reporter{"report", compile};
        reporter.write(rep_cfg, result);

    } catch (const std::exception& e) {
        std::cerr << "\nError: " << e.what() << "\n";
        return 1;
    }

    return 0;
}