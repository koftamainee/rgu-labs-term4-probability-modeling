//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_ICLASSIFIER_H
#define SPAM_CLASSIFIER_ICLASSIFIER_H
#include <unordered_map>

#include "IModel.h"

namespace model {

template <class TInput, class TLabel>
class IClassifier : public IModel<TInput, TLabel> {
public:
  virtual std::unordered_map<TLabel, double>
  predict_probability(const TInput& input) const = 0;
};

}

#endif //SPAM_CLASSIFIER_ICLASSIFIER_H