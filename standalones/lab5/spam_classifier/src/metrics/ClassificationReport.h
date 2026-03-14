//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_CLASSIFICATIONREPORT_H
#define SPAM_CLASSIFIER_CLASSIFICATIONREPORT_H
#include <unordered_map>

#include "ConfusionMatrix.h"

namespace metrics {
template <typename TLabel>
struct PerClassMetrics {
  double precision = 0.0;
  double recall = 0.0;
  double f1 = 0.0;
  int support = 0;
};

template <typename TLabel>
class ClassificationReport {
public:
  std::unordered_map<TLabel, PerClassMetrics<TLabel>> per_class;
  double accuracy = 0.0;
  double macro_f1 = 0.0;
  double weighted_f1 = 0.0;

  void print() const;
};

template <typename TLabel>
ClassificationReport<TLabel> make_classification_report(
    const ConfusionMatrix<TLabel>& cm) {}
}

#endif //SPAM_CLASSIFIER_CLASSIFICATIONREPORT_H