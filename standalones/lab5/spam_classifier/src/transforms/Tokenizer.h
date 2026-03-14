//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_TOKENIZER_H
#define SPAM_CLASSIFIER_TOKENIZER_H
#include <fstream>
#include <string>
#include <vector>

#include "ITransform.h"

namespace transform {
class Tokenizer : public ITransform<std::string, std::vector<std::string>> {
public:
  std::vector<std::string> operator()(const std::string& input) const override {
    std::vector<std::string> tokens;
    std::ifstream stream(input);
    if (!stream) {
      throw std::runtime_error("Could not open file");
    }
    std::string token;
    while (stream >> token) {
      tokens.push_back(token);
    }
    return tokens;
  }
};
}
#endif //SPAM_CLASSIFIER_TOKENIZER_H