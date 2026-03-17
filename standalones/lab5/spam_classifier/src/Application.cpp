//
// Created by koftamainee on 3/16/26.
//
#include "Application.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

std::unordered_set<std::string> load_stop_words_from_csv(
    const std::string& path) {
  if (path.empty()) {
    return {};
  }
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  std::unordered_set<std::string> stop_words;
  std::string line;

  while (std::getline(file, line)) {
    std::istringstream ss(line);
    std::string word;
    while (std::getline(ss, word, ',')) {
      auto start = word.find_first_not_of(" \t\r\n");
      auto end = word.find_last_not_of(" \t\r\n");
      if (start != std::string::npos)
        stop_words.insert(word.substr(start, end - start + 1));
    }
  }

  return stop_words;
}

void print_help() {
  std::cout <<
      "Usage: program [MODE] [OPTIONS]\n"
      "\n"
      "Modes:\n"
      "  -t, --train      Train the model\n"
      "  -p, --predict    Run prediction\n"
      "\n"
      "Options:\n"
      "  -d, --data       <path>   Path to input data CSV\n"
      "  -m, --model      <path>   Path to model file\n"
      "  -o, --output     <path>   Path to output file\n"
      "  -s, --stop-words <path>   Path to stop words CSV (optional)\n"
      "  -a, --alpha      <value>  Smoothing parameter, default: 1.0 (optional)\n"
      "\n"
      "Examples:\n"
      "  program --train  --data data.csv --model model.bin --output out.csv\n"
      "  program --predict -d data.csv -m model.bin -o out.csv -s stops.txt -a 0.5\n";
}

std::vector<std::string> load_emails_from_csv(const std::string& path) {
  std::ifstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  std::vector<std::string> emails;
  std::string header;

  if (!std::getline(file, header)) {
    throw std::runtime_error("Empty CSV file");
  }

  std::istringstream header_ss(header);
  std::string col;
  size_t email_idx = std::string::npos;
  size_t idx = 0;

  while (std::getline(header_ss, col, ',')) {
    if (col == "email_text") {
      email_idx = idx;
      break;
    }
    idx++;
  }

  if (email_idx == std::string::npos) {
    throw std::runtime_error("email_text column not found");
  }

  std::string line;

  while (true) {
    std::string row;
    size_t quote_count = 0;
    while (std::getline(file, line)) {
      row += line + "\n";
      quote_count += std::count(line.begin(), line.end(), '"');

      if (quote_count % 2 == 0)
        break;
    }

    if (row.empty())
      break;

    if (row.back() == '\n')
      row.pop_back();

    std::istringstream ss(row);
    std::string field;
    size_t col_idx = 0;

    while (std::getline(ss, field, ',')) {
      if (col_idx == email_idx) {
        emails.push_back(field);
        break;
      }
      col_idx++;
    }
  }

  return emails;
}

void write_results_to_csv(const std::string& path,
                          const std::vector<bool>& is_spam) {
  std::ofstream file(path);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open file: " + path);
  }

  for (std::size_t i = 0; i < is_spam.size(); ++i) {
    file << (is_spam[i] ? "spam" : "ham");
    if (i + 1 < is_spam.size())
      file << '\n';
  }
}

Configuration Configuration::load(int argc, char* argv[]) {
  Configuration config;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];

    auto next = [&]() -> std::string {
      if (i + 1 >= argc)
        throw std::invalid_argument("Missing value for argument: " + arg);
      return argv[++i];
    };

    if (arg == "--train" || arg == "-t") {
      config.mode = Mode::Train;
    } else if (arg == "--predict" || arg == "-p") {
      config.mode = Mode::Predict;
    } else if (arg == "--data" || arg == "-d") {
      config.data_path = next();
    } else if (arg == "--model" || arg == "-m") {
      config.model_path = next();
    } else if (arg == "--output" || arg == "-o") {
      config.output_path = next();
    } else if (arg == "--stop-words" || arg == "-s") {
      config.stop_words_path = next();
    } else if (arg == "--alpha" || arg == "-a") {
      config.alpha = std::stod(next());
    } else {
      throw std::invalid_argument("Unknown argument: " + arg);
    }
  }

  return config;
}

bool Configuration::is_complete() const {
  if (data_path.empty() || model_path.empty()) {
    return false;
  }

  if (output_path.empty() && mode == Mode::Predict) {
    return false;
  }

  if (alpha < 0.0) {
    return false;
  }

  return true;
}