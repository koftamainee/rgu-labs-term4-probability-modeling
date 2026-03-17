//
// Created by koftamainee on 3/16/26.
//

#ifndef SPAM_CLASSIFIER_APPLICATION_H
#define SPAM_CLASSIFIER_APPLICATION_H
#include <string>
#include <unordered_set>
#include <vector>

std::unordered_set<std::string> load_stop_words_from_csv(const std::string &path);
std::vector<std::string> load_emails_from_csv(const std::string& path);
void write_results_to_csv(const std::string &path, const std::vector<bool>& is_spam);

void print_help();

class Configuration final {
public:
  enum class Mode {
    Train,
    Predict,
  };

  Mode mode = Mode::Train;
  std::string data_path{};
  std::string model_path{};
  std::string output_path{};
  std::string stop_words_path{};
  double alpha = 1.0;

  static Configuration load(int argc, char *argv[]);
  bool is_complete() const;

};

#endif //SPAM_CLASSIFIER_APPLICATION_H