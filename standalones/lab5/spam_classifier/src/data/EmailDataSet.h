//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_EMAILDATASET_H
#define SPAM_CLASSIFIER_EMAILDATASET_H
#include <vector>

#include "IDataSet.h"

namespace data {
class EmailDataSet :public IDataSet<std::vector<std::string>, bool> {
public:
  explicit EmailDataSet(std::vector<Sample> samples)
      : m_samples(std::move(samples)) {}

  Sample operator[](size_t idx) const override { return m_samples.at(idx); }
  size_t size()                 const override { return m_samples.size(); }

private:
  std::vector<Sample> m_samples;

};
}

#endif //SPAM_CLASSIFIER_EMAILDATASET_H