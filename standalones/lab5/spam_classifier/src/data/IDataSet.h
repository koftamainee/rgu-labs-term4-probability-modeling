//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_DATASET_H
#define SPAM_CLASSIFIER_DATASET_H
#include "LabledSample.h"

namespace data {
template <typename TData, typename TLabel>
class IDataSet {
public:
  using Sample = LabeledSample<TData, TLabel>;

  virtual Sample operator[](size_t idx) const = 0;
  virtual size_t size() const = 0;
  virtual ~IDataSet() = default;
};
}

#endif //SPAM_CLASSIFIER_DATASET_H