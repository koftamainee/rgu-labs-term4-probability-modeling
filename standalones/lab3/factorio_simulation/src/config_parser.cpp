#include "config_parser.hpp"

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include "recipe.hpp"

static std::string trim(const std::string& s) {
    const auto b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return {};
    const auto e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

static bool is_comment_or_blank(const std::string& line) {
    const std::string t = trim(line);
    return t.empty() || t[0] == '#';
}

static bool is_indented(const std::string& line) {
    return !line.empty() && (line[0] == ' ' || line[0] == '\t');
}

static std::pair<std::string, std::string> split_kv(const std::string& line) {
    const auto pos = line.find('=');
    if (pos == std::string::npos)
        throw std::runtime_error("Expected '=' in: " + line);
    return { trim(line.substr(0, pos)), trim(line.substr(pos + 1)) };
}

static std::pair<std::string, int> parse_named_int(const std::string& line) {
    const auto pos = line.find(':');
    if (pos == std::string::npos)
        throw std::runtime_error("Expected 'Name : value' in: " + line);
    const std::string name  = trim(line.substr(0, pos));
    const std::string value = trim(line.substr(pos + 1));
    if (name.empty())
        throw std::runtime_error("Empty name in: " + line);
    try { return { name, std::stoi(value) }; }
    catch (...) { throw std::runtime_error("Expected integer after ':' in: " + line); }
}

static int require_int(const std::map<std::string, std::string>& kv,
                        const std::string& key) {
    const auto it = kv.find(key);
    if (it == kv.end())
        throw std::runtime_error("Missing required field: " + key);
    try { return std::stoi(it->second); }
    catch (...) { throw std::runtime_error("Field '" + key + "' must be an integer"); }
}

static std::string require_str(const std::map<std::string, std::string>& kv,
                                const std::string& key) {
    const auto it = kv.find(key);
    if (it == kv.end())
        throw std::runtime_error("Missing required field: " + key);
    return it->second;
}

FactoryConfig parse_config(const std::filesystem::path& path) {
    std::ifstream f(path);
    if (!f) throw std::runtime_error("Cannot open config: " + path.string());

    std::map<std::string, std::string>              scalars;
    std::vector<std::pair<std::string, int>>        recipe_block;
    std::vector<std::pair<std::string, int>>        supply_block;
    std::vector<std::pair<std::string, int>>*       active_block = nullptr;

    std::vector<std::string> lines;
    {
        std::string line;
        while (std::getline(f, line)) lines.push_back(line);
    }

    for (std::size_t i = 0; i < lines.size(); ++i) {
        const std::string& raw = lines[i];

        if (is_comment_or_blank(raw)) continue;

        if (is_indented(raw)) {
            const std::string t = trim(raw);
            if (is_comment_or_blank(t)) continue;
            if (!active_block)
                throw std::runtime_error("Unexpected indented line: " + raw);
            active_block->push_back(parse_named_int(t));
            continue;
        }

        active_block = nullptr;

        const auto [key, value] = split_kv(raw);

        if (key == "recipe") {
            active_block = &recipe_block;
        } else if (key == "supply") {
            active_block = &supply_block;
        } else {
            scalars[key] = value;
        }
    }

    if (recipe_block.empty())
        throw std::runtime_error("Config must have a non-empty 'recipe' block");
    if (supply_block.empty())
        throw std::runtime_error("Config must have a non-empty 'supply' block");

    std::map<std::string, int> supply_map;
    for (const auto& [name, rate] : supply_block)
        supply_map[name] = rate;

    std::vector<Ingredient> ingredients;
    std::vector<int>        supply_rates;
    for (int i = 0; i < static_cast<int>(recipe_block.size()); ++i) {
        const auto& [name, count] = recipe_block[i];
        const auto sit = supply_map.find(name);
        if (sit == supply_map.end())
            throw std::runtime_error("No supply rate defined for ingredient: " + name);
        ingredients.push_back(Ingredient{i, count, name});
        supply_rates.push_back(sit->second);
    }

    const std::string product_name = scalars.count("product")
                                   ? scalars["product"] : "Product";
    const int craft_time   = require_int(scalars, "craft_time");
    const int recycle_time = require_int(scalars, "recycle_time");
    const int assemblers   = require_int(scalars, "assemblers");
    const int recyclers    = require_int(scalars, "recyclers");
    const int asm_modules  = require_int(scalars, "assembler_modules");
    const int rec_modules  = require_int(scalars, "recycler_modules");

    Recipe recipe{ingredients, product_name};

    SimulationConfig sim{
        assemblers,
        recyclers,
        asm_modules,
        rec_modules,
        craft_time,
        recycle_time,
        recipe,
        supply_rates
    };

    ReportConfig report{
        assemblers,
        recyclers,
        asm_modules,
        rec_modules,
        craft_time,
        recycle_time,
        Simulation::LEGENDARY_GOAL,
        ingredients,
        supply_rates
    };

    return FactoryConfig{sim, report};
}