//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_ITRANSFORM_H
#define SPAM_CLASSIFIER_ITRANSFORM_H

namespace transform {
template <typename TIn, typename TOut>
class ITransform {
public:
  using input_type = TIn;
  using output_type = TOut;
  virtual ~ITransform() = default;

  virtual TOut operator()(const TIn &input) const = 0;
};
}

#endif //SPAM_CLASSIFIER_ITRANSFORM_H