#ifndef FACTORIOSIMULATION_QUALITY_HPP
#define FACTORIOSIMULATION_QUALITY_HPP

#include <array>
#include <random>
#include <stdexcept>
#include <string_view>

enum class Quality : int {
    Normal    = 1,
    Uncommon  = 2,
    Rare      = 3,
    Epic      = 4,
    Legendary = 5,
};

constexpr int QUALITY_MIN     = static_cast<int>(Quality::Normal);
constexpr int QUALITY_MAX     = static_cast<int>(Quality::Legendary);
constexpr int QUALITY_COUNT   = QUALITY_MAX - QUALITY_MIN + 1;
constexpr int MAX_QUALITY_MODULES = 4;

constexpr double MODULE_SPEED_PENALTY = 0.10;

constexpr std::string_view quality_name(Quality q) noexcept {
    switch (q) {
        case Quality::Normal:    return "Normal";
        case Quality::Uncommon:  return "Uncommon";
        case Quality::Rare:      return "Rare";
        case Quality::Epic:      return "Epic";
        case Quality::Legendary: return "Legendary";
    }
    return "Unknown";
}

constexpr std::string_view quality_color(Quality q) noexcept {
    switch (q) {
        case Quality::Normal:    return "gray";
        case Quality::Uncommon:  return "green!60!black";
        case Quality::Rare:      return "blue!70!black";
        case Quality::Epic:      return "violet";
        case Quality::Legendary: return "orange!90!black";
    }
    return "black";
}

constexpr double single_module_upgrade_prob(int k) noexcept {
    double p = 52.0;
    for (int i = 0; i < 2 + k; i++) { p /= 10.0; }
    return p;
}

Quality quality_from_int(int v);

class QualityModuleSet final {
public:
    explicit QualityModuleSet(int count = 0);

    void set_count(int count);
    int count() const noexcept;
    double speed_multiplier() const noexcept;
    const std::array<double, 4>& upgrade_probs() const noexcept;
    double total_upgrade_prob() const noexcept;

    template <typename RNG>
    Quality roll(Quality input, RNG& rng) const {
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        const double r = dist(rng);
        const int input_level = static_cast<int>(input);
        double cumulative = 0.0;

        for (int i = 1; i <= 4; i++) {
            const int result_level = input_level + i;
            if (result_level > QUALITY_MAX) { break; }
            cumulative += m_upgrade_probs[i - 1];
            if (r < cumulative) {
                return quality_from_int(result_level);
            }
        }

        return input;
    }

private:
    int m_count = 0;
    std::array<double, 4> m_upgrade_probs{};

    void recompute() noexcept;
};

#endif // FACTORIOSIMULATION_QUALITY_HPP