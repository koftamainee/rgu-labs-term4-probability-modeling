//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_CLASSIFICATIONREPORT_H
#define SPAM_CLASSIFIER_CLASSIFICATIONREPORT_H
#include <iomanip>
#include <unordered_map>

#include "ConfusionMatrix.h"

namespace metrics {
struct PerClassMetrics {
  double precision = 0.0;
  double recall = 0.0;
  double f1 = 0.0;
  int support = 0;
};

template <typename TLabel>
class ClassificationReport {
public:
  std::unordered_map<TLabel, PerClassMetrics> per_class;
  double accuracy = 0.0;
  double macro_f1 = 0.0;
  double weighted_f1 = 0.0;

  void print() const {
    std::cout << "Accuracy: " << accuracy << "\n";
    std::cout << "Macro F1: " << macro_f1 << "\n";
    std::cout << "Weighted F1: " << weighted_f1 << "\n\n";

    std::cout << std::left
              << std::setw(10) << "Class"
              << std::setw(12) << "Precision"
              << std::setw(12) << "Recall"
              << std::setw(12) << "F1"
              << std::setw(8) << "Support"
              << "\n";

    std::cout << std::string(54, '-') << "\n";

    for (const auto& [label, metrics] : per_class) {
      std::cout << std::left
                << std::setw(10) << label
                << std::setw(12) << metrics.precision
                << std::setw(12) << metrics.recall
                << std::setw(12) << metrics.f1
                << std::setw(8) << metrics.support
                << "\n";
    }
  }
};

template <typename TLabel>
ClassificationReport<TLabel> make_classification_report(
    const ConfusionMatrix<TLabel>& cm) {
  ClassificationReport<TLabel> report;

  int total_correct = 0;
  const int total_examples = cm.total();

  double f1_sum = 0.0;
  double weighted_f1_sum = 0.0;

  for (const auto& actual_pair : cm.matrix) {
    TLabel label = actual_pair.first;
    const int TP = cm.tp(label);
    const int FP = cm.fp(label);
    const int FN = cm.fn(label);

    PerClassMetrics metrics;
    metrics.precision =
        TP + FP == 0 ? 0.0 : static_cast<double>(TP) / (TP + FP);
    metrics.recall = TP + FN == 0 ? 0.0 : static_cast<double>(TP) / (TP + FN);
    metrics.f1 = metrics.precision + metrics.recall == 0
                   ? 0.0
                   : 2.0 * metrics.precision * metrics.recall / (
                       metrics.precision + metrics.recall);
    metrics.support = TP + FN;

    report.per_class[label] = metrics;

    total_correct += TP;
    f1_sum += metrics.f1;
    weighted_f1_sum += metrics.f1 * metrics.support;
  }

  report.accuracy = static_cast<double>(total_correct) / total_examples;
  report.macro_f1 = f1_sum / cm.matrix.size();
  report.weighted_f1 = weighted_f1_sum / total_examples;

  return report;
}
}

#endif //SPAM_CLASSIFIER_CLASSIFICATIONREPORT_H