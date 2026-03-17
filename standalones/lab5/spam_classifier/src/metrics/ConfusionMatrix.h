//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_CONFUSIONMATRIX_H
#define SPAM_CLASSIFIER_CONFUSIONMATRIX_H
#include <unordered_map>
#include "model/IClassifier.h"

namespace metrics {
template <typename TLabel>
struct ConfusionMatrix {
  std::unordered_map<TLabel, std::unordered_map<TLabel, int>> matrix;

  int total() const {
    int sum = 0;
    for (const auto& actual_pair : matrix) {
      for (const auto& pred_pair : actual_pair.second) {
        sum += pred_pair.second;
      }
    }
    return sum;
  }

  int count(TLabel actual, TLabel predicted) const {
    auto it1 = matrix.find(actual);
    if (it1 == matrix.end()) { return 0; }
    auto it2 = it1->second.find(predicted);
    return it2 != it1->second.end() ? it2->second : 0;
  }

  int tp(TLabel label) const {
    return count(label, label);
  }

  int fn(TLabel label) const {
    int sum = 0;
    auto it = matrix.find(label);
    if (it != matrix.end()) {
      for (const auto& pair : it->second) {
        if (pair.first != label)
          sum += pair.second;
      }
    }
    return sum;
  }

  int fp(TLabel label) const {
    int sum = 0;
    for (const auto& actual_pair : matrix) {
      if (actual_pair.first == label) continue;
      auto it = actual_pair.second.find(label);
      if (it != actual_pair.second.end())
        sum += it->second;
    }
    return sum;
  }

  int tn(TLabel label) const {
    return total() - tp(label) - fn(label) - fp(label);
  }
};

template <typename TInput, typename TLabel>
ConfusionMatrix<TLabel> make_confusion_matrix(
    model::IClassifier<TInput, TLabel>& model,
    const data::IDataSet<TInput, TLabel>& dataset) {

  ConfusionMatrix<TLabel> cm;

  for (size_t i = 0; i < dataset.size(); ++i) {
    const auto& sample = dataset[i];
    const TInput& x = sample.data;
    const TLabel& y_true = sample.label;
    const TLabel y_pred = model.predict(x);

    cm.matrix[y_true][y_pred] += 1;
  }

  return cm;
}
}

#endif //SPAM_CLASSIFIER_CONFUSIONMATRIX_H