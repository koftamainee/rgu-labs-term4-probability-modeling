//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_REMOVESTOPWORDS_H
#define SPAM_CLASSIFIER_REMOVESTOPWORDS_H
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "ITransform.h"

namespace transform {
class RemoveStopWords : public ITransform<
      std::vector<std::string>, std::vector<std::string>> {
public:
  explicit
  RemoveStopWords(std::unordered_set<std::string> stop_words) : m_stop_words(
      std::move(stop_words)) {}

  std::vector<std::string> operator()(
      const std::vector<std::string>& input) const override {
    std::vector<std::string> output;

    for (const auto& token : input) {
      if (!m_stop_words.contains(token)) {
        output.push_back(token);
      }
    }
    return output;
  }

private:
  std::unordered_set<std::string> m_stop_words;
};
}
#endif //SPAM_CLASSIFIER_REMOVESTOPWORDS_H