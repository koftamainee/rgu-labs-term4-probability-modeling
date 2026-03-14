//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_NAIVEBAYES_H
#define SPAM_CLASSIFIER_NAIVEBAYES_H
#include "IClassifier.h"

namespace model {
class EmailBayes : public IClassifier<std::vector<std::string>, bool> {
public:
  explicit EmailBayes(double alpha = 1.0);

  void load(const std::string& path) override;
  void fit(
      const data::IDataSet<std::vector<std::string>, bool> &dataset) override;

  bool predict(const std::vector<std::string>& input) const override;

  std::unordered_map<bool, double> predict_probability(
      const std::vector<std::string>& input) const override;
};
}

#endif //SPAM_CLASSIFIER_NAIVEBAYES_H