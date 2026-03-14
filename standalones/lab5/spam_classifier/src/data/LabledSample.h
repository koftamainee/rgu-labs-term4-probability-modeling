//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_LABLEDSAMPLE_H
#define SPAM_CLASSIFIER_LABLEDSAMPLE_H
#include <string>
#include <vector>

namespace data {
template <typename TData, typename TLabel>
struct LabeledSample {
  TData data;
  TLabel label;
};
}

#endif //SPAM_CLASSIFIER_LABLEDSAMPLE_H