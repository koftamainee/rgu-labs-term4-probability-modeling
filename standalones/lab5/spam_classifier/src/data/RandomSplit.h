//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_RANDOMSPLIT_H
#define SPAM_CLASSIFIER_RANDOMSPLIT_H
#include <algorithm>
#include <random>
#include <stdexcept>

#include "Subset.h"

namespace data {
template <typename TData, typename TLabel,
          std::uniform_random_bit_generator TRNG = std::mt19937>
std::pair<Subset<TData, TLabel>, Subset<TData, TLabel>>
random_split(const std::shared_ptr<IDataSet<TData, TLabel>> dataset,
             double train_ratio = 0.7,
             TRNG rng = TRNG{std::random_device{}()}) {
  if (train_ratio <= 0.0 || train_ratio >= 1.0)
    throw std::invalid_argument("trainRatio must be in (0, 1)");

  const size_t n = dataset->size();
  const auto train_size = static_cast<size_t>(n * train_ratio);

  std::vector<size_t> indices(n);
  std::iota(indices.begin(), indices.end(), 0);
  std::shuffle(indices.begin(), indices.end(), rng);

  return {
      Subset<TData, TLabel>{dataset, {indices.begin(),
                                      indices.begin() + train_size}},
      Subset<TData, TLabel>{dataset, {indices.begin() + train_size,
                                      indices.end()}}
  };
}
}

#endif //SPAM_CLASSIFIER_RANDOMSPLIT_H