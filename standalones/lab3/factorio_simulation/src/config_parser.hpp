#ifndef FACTORIOSIMULATION_CONFIG_PARSER_HPP
#define FACTORIOSIMULATION_CONFIG_PARSER_HPP


#include "reporter.hpp"
#include "simulation.hpp"
#include <string>
#include <filesystem>

struct FactoryConfig {
  SimulationConfig  sim;
  ReportConfig      report;
};

FactoryConfig parse_config(const std::filesystem::path& path);

#endif