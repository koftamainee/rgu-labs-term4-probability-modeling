//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_SUBSET_H
#define SPAM_CLASSIFIER_SUBSET_H
#include "IDataSet.h"

namespace data {
template <typename TData, typename TLabel>
class Subset : public IDataSet<TData, TLabel> {
public:
  using Sample = LabeledSample<TData, TLabel>;

  Subset(const std::shared_ptr<IDataSet<TData, TLabel>> dataset, std::vector<size_t> indices)
      : m_dataset(dataset), m_indices(std::move(indices)) {}

  Sample operator[](size_t idx) const override {
    return m_dataset[m_indices.at(idx)];
  }

  size_t size() const override { return m_indices.size(); }

  const std::vector<size_t>& indices() const { return m_indices; }

private:
  const std::shared_ptr<IDataSet<TData, TLabel>> m_dataset;
  std::vector<size_t>           m_indices;
};
}

#endif //SPAM_CLASSIFIER_SUBSET_H