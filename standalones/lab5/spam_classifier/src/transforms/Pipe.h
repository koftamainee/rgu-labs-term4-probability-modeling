//
// Created by koftamainee on 3/14/26.
//

#ifndef SPAM_CLASSIFIER_PIPE_H
#define SPAM_CLASSIFIER_PIPE_H
#include <memory>

#include "ITransform.h"

namespace transform {
template <typename TIn, typename TMid, typename TOut>
class Pipe : public ITransform<TIn, TOut> {
public:
  Pipe(std::unique_ptr<ITransform<TIn, TMid>> first,
       std::unique_ptr<ITransform<TMid, TOut>> second)
    : m_first(std::move(first)), m_second(std::move(second)) {}

  TOut operator()(const TIn& input) const override {
    return (*m_second)((*m_first)(input));
  }

private:
  std::unique_ptr<ITransform<TIn, TMid>> m_first;
  std::unique_ptr<ITransform<TMid, TOut>> m_second;
};


template <typename TFirst, typename TSecond>
auto make_pipe(std::unique_ptr<TFirst>  first,
               std::unique_ptr<TSecond> second) {
  using TIn  = typename TFirst::input_type;
  using TMid = typename TFirst::output_type;
  using TOut = typename TSecond::output_type;

  static_assert(std::is_same_v<TMid, typename TSecond::input_type>,
                "make_pipe: output type of first must match input type of second");

  return std::make_unique<Pipe<TIn, TMid, TOut>>(
      std::unique_ptr<ITransform<TIn, TMid>>(std::move(first)),
      std::unique_ptr<ITransform<TMid, TOut>>(std::move(second))
  );
}
}


#endif //SPAM_CLASSIFIER_PIPE_H