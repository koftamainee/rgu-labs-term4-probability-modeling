//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_LOWERCASE_H
#define SPAM_CLASSIFIER_LOWERCASE_H
#include <algorithm>
#include <string>
#include <vector>

#include "ITransform.h"

namespace transform {
class Lowercase : public ITransform<
      std::vector<std::string>, std::vector<std::string>> {
public:
  std::vector<std::string> operator()(
      const std::vector<std::string>& input) const override {
    std::vector<std::string> result;

    for (const auto& token : input) {
      std::string lowered = token;
      std::ranges::transform(lowered, lowered.begin(), ::tolower);
      result.push_back(lowered);
    }
    return result;
  }
};
}
#endif //SPAM_CLASSIFIER_LOWERCASE_H