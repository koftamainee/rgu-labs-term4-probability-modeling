//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_IMODEL_H
#define SPAM_CLASSIFIER_IMODEL_H
#include "data/IDataSet.h"

namespace model {
template <typename TInput, typename TOutput>
class IModel {
public:
  virtual ~IModel() = default;

  virtual void load(const std::string &path) = 0;
  virtual void save(const std::string &path) = 0;

  virtual void fit(const data::IDataSet<TInput, TOutput> &dataset) = 0;
  virtual TOutput predict(const TInput& input) const = 0;
};
}

#endif //SPAM_CLASSIFIER_IMODEL_H