//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_DATALOADER_H
#define SPAM_CLASSIFIER_DATALOADER_H
#include <memory>
#include "IDataSet.h"
#include "../transforms/Lowercase.h"

namespace data {
template <typename TDataIn, typename TDataOut, typename TLabel>
class IDataLoader {
public:
  virtual ~IDataLoader() = default;

  virtual std::shared_ptr<IDataSet<TDataOut, TLabel>>
  load(const std::string& source) const = 0;

  virtual std::shared_ptr<IDataSet<TDataOut, TLabel>>
  load(const std::string& source,
       const transform::ITransform<TDataIn, TDataOut>& transform) const = 0;
};
}

#endif //SPAM_CLASSIFIER_DATALOADER_H