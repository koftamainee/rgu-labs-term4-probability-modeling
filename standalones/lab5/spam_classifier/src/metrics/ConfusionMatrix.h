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

  int total() const;
  int count(TLabel actual, TLabel predicted) const;
};

template <typename TInput, typename TLabel>
ConfusionMatrix<TLabel> make_confusion_matrix(
    model::IClassifier<TInput, TLabel> &model,
    const data::IDataSet<TInput, TLabel> &dataset);
}

#endif //SPAM_CLASSIFIER_CONFUSIONMATRIX_H