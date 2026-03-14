//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_EMAILDATALOADER_H
#define SPAM_CLASSIFIER_EMAILDATALOADER_H
#include "IDataLoader.h"

namespace data {
class EmailDataLoader : public IDataLoader<
      std::string, std::vector<std::string>, bool> {
public:
  explicit EmailDataLoader();

  std::unique_ptr<IDataSet<std::vector<std::string>, bool>>
  load(const std::string& path) const override;

  std::shared_ptr<IDataSet<std::vector<std::string>, bool>> load(
      const std::string& source,
      const transform::ITransform<
        std::string, std::vector<std::string>>
      & transform) const
  override;
};
}

#endif //SPAM_CLASSIFIER_EMAILDATALOADER_H