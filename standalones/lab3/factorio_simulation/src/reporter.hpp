#ifndef FACTORIOSIMULATION_REPORTER_HPP
#define FACTORIOSIMULATION_REPORTER_HPP

#include "simulation.hpp"
#include <string>
#include <filesystem>

struct ReportConfig {
  int assembler_count;
  int recycler_count;
  int assembler_quality_modules;
  int recycler_quality_modules;
  int base_craft_time;
  int base_recycle_time;
  int legendary_goal;
  std::vector<Ingredient> ingredients;
  std::vector<int> supply_rates;
};

class Reporter final {
public:
  explicit Reporter(const std::filesystem::path& output_path,
                    bool compile_pdf = true);

  void write(const ReportConfig& config, const SimulationResult& result) const;

private:
  std::filesystem::path m_tex_path;
  std::filesystem::path m_pdf_path;
  bool m_compile;

  static std::string escape(const std::string& s);

  static std::string build_pgfplot_coords(const std::vector<int>& ticks);

  static std::string format_ticks_table(const std::vector<int>& ticks,
                                        int cols = 8);

  static std::string build_document(const ReportConfig& config,
                             const SimulationResult& result) ;
};

#endif // FACTORIOSIMULATION_REPORTER_HPP