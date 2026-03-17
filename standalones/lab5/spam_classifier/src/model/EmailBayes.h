//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_NAIVEBAYES_H
#define SPAM_CLASSIFIER_NAIVEBAYES_H
#include <unordered_set>

#include "IClassifier.h"

namespace model {
class EmailBayes : public IClassifier<std::vector<std::string>, bool> {
public:
  explicit EmailBayes(double alpha = 1.0);

  void load(const std::string& path) override;
  void save(const std::string& path) override;
  void fit(
      const data::IDataSet<std::vector<std::string>, bool>& dataset) override;

  bool predict(const std::vector<std::string>& input) const override;

  std::unordered_map<bool, double> predict_probabilities(
      const std::vector<std::string>& input) const override;

private:
  std::unordered_map<bool, double> predict_log_probabilities(
      const std::vector<std::string>& input) const;

private:
  double m_alpha;
  double m_prior_spam = 0.0;
  double m_prior_ham = 1.0;
  size_t m_total_spam_words = 0;
  size_t m_total_ham_words = 0;
  std::unordered_set<std::string> m_vocabulary;
  std::unordered_map<std::string, double> m_likelihood_spam;
  std::unordered_map<std::string, double> m_likelihood_ham;
};
}

#endif //SPAM_CLASSIFIER_NAIVEBAYES_H