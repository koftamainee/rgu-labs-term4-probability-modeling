//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_STRIPPUNCTUATION_H
#define SPAM_CLASSIFIER_STRIPPUNCTUATION_H
#include <algorithm>
#include <string>
#include <vector>

#include "ITransform.h"

namespace transform {
class StripPunctuation : public ITransform<
      std::vector<std::string>, std::vector<std::string>> {
public:
  std::vector<std::string> operator()(
      const std::vector<std::string>& input) const override {
    std::vector<std::string> output;
    output.reserve(input.size());

    for (const auto& token : input) {
      std::string stripped;
      std::ranges::copy_if(token, std::back_inserter(stripped),
                           [](unsigned char c) {
                             return !std::ispunct(c);
                           });
      if (!stripped.empty()) {
        output.emplace_back(stripped);
      }
    }
    return output;
  }
};
}
#endif //SPAM_CLASSIFIER_STRIPPUNCTUATION_H