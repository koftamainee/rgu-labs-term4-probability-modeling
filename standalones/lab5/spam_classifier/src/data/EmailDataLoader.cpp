//
// Created by koftamainee on 3/14/26.
//

#include "EmailDataLoader.h"
#include "EmailDataSet.h"

#include <fstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <cctype>
#include <algorithm>
#include <iomanip>

namespace {
std::string normalize(std::string s) {
  auto is_space = [](unsigned char c) { return std::isspace(c); };

  while (!s.empty() && (is_space(s.front()) || s.front() == '"' || s.front() ==
                        '\r'))
    s.erase(s.begin());

  while (!s.empty() && (is_space(s.back()) || s.back() == '"' || s.back() ==
                        '\r'))
    s.pop_back();

  for (char& c : s)
    c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));

  return s;
}

std::vector<std::string> parse_csv_row(const std::string& line) {
  std::vector<std::string> result;
  std::string field;
  bool in_quotes = false;

  for (size_t i = 0; i < line.size(); ++i) {
    if (const char c = line[i]; c == '"') {
      if (in_quotes && i + 1 < line.size() && line[i + 1] == '"') {
        field += '"';
        ++i;
      } else {
        in_quotes = !in_quotes;
      }
    } else if (c == ',' && !in_quotes) {
      result.push_back(field);
      field.clear();
    } else {
      field += c;
    }
  }

  result.push_back(field);
  return result;
}

std::pair<size_t, size_t> find_columns(const std::vector<std::string>& header) {
  size_t text_idx = std::string::npos;
  size_t label_idx = std::string::npos;

  for (size_t i = 0; i < header.size(); ++i) {
    std::string col = normalize(header[i]);
    if (col == "email_text") text_idx = i;
    if (col == "label") label_idx = i;
  }

  if (text_idx == std::string::npos || label_idx == std::string::npos)
    throw std::runtime_error("Required columns not found");

  return {text_idx, label_idx};
}

std::pair<bool, std::string> parse_row(const std::string& line, size_t text_idx,
                                       size_t label_idx) {
  const auto cols = parse_csv_row(line);

  if (text_idx >= cols.size() || label_idx >= cols.size())
    throw std::runtime_error("Malformed CSV row: " + line);

  std::string text = cols[text_idx];
  std::string label = normalize(cols[label_idx]);

  if (label != "spam" && label != "ham")
    throw std::runtime_error("Unknown label: " + label);

  return {label == "spam", text};
}

std::string read_csv_row(std::ifstream& file) {
  std::string line, full_line;
  size_t quote_count = 0;

  while (std::getline(file, line)) {
    full_line += line + "\n";
    quote_count += std::ranges::count(line, '"');

    if (quote_count % 2 == 0) {
      if (!full_line.empty()) full_line.pop_back();
      break;
    }
  }

  if (full_line.empty()) return "";
  return full_line;
}
} // namespace

namespace data {
EmailDataLoader::EmailDataLoader() = default;

std::shared_ptr<IDataSet<std::vector<std::string>, bool>>
EmailDataLoader::load(const std::string& path) const {
  std::ifstream file(path);
  if (!file.is_open()) throw std::runtime_error("Failed to open file: " + path);

  std::string header_line;
  if (!std::getline(file, header_line)) throw std::runtime_error(
      "Empty CSV file");

  if (header_line.size() >= 3 &&
      static_cast<unsigned char>(header_line[0]) == 0xEF &&
      static_cast<unsigned char>(header_line[1]) == 0xBB &&
      static_cast<unsigned char>(header_line[2]) == 0xBF) {
    header_line.erase(0, 3);
  }

  auto header = parse_csv_row(header_line);
  auto [text_idx, label_idx] = find_columns(header);

  std::vector<EmailDataSet::Sample> samples;

  while (true) {
    std::string row = read_csv_row(file);
    if (row.empty()) break;

    auto [label, text] = parse_row(row, text_idx, label_idx);
    samples.push_back({{text}, label});
  }

  return std::make_shared<EmailDataSet>(std::move(samples));
}

std::shared_ptr<IDataSet<std::vector<std::string>, bool>>
EmailDataLoader::load(
    const std::string& source,
    const transform::ITransform<std::string, std::vector<std::string>>&
    transform) const {
  std::ifstream file(source);
  if (!file.is_open()) throw std::runtime_error(
      "Failed to open file: " + source);

  std::string header_line;
  if (!std::getline(file, header_line)) throw std::runtime_error(
      "Empty CSV file");

  if (header_line.size() >= 3 &&
      static_cast<unsigned char>(header_line[0]) == 0xEF &&
      static_cast<unsigned char>(header_line[1]) == 0xBB &&
      static_cast<unsigned char>(header_line[2]) == 0xBF) {
    header_line.erase(0, 3);
  }

  auto header = parse_csv_row(header_line);
  auto [text_idx, label_idx] = find_columns(header);

  std::vector<EmailDataSet::Sample> samples;

  while (true) {
    std::string row = read_csv_row(file);
    if (row.empty()) break;

    auto [label, text] = parse_row(row, text_idx, label_idx);
    samples.push_back({transform(text), label});
  }

  return std::make_shared<EmailDataSet>(std::move(samples));
}
} // namespace data2