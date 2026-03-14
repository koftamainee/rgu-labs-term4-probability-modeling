#ifndef SPAM_CLASSIFIER_COMPOSE_H
#define SPAM_CLASSIFIER_COMPOSE_H

#include <memory>
#include <vector>
#include "ITransform.h"

namespace transform {
template <typename T>
class Sequential : public ITransform<T, T> {
public:
  explicit Sequential(std::vector<std::unique_ptr<ITransform<T, T>>> transforms)
    : m_transforms(std::move(transforms)) {}

  T operator()(const T& input) const override {
    T result = input;
    for (const auto& transform : m_transforms)
      result = (*transform)(result);
    return result;
  }

private:
  std::vector<std::unique_ptr<ITransform<T, T>>> m_transforms;
};

template <typename T = void, typename... Args>
auto make_sequential(Args&&... args) {
  if constexpr (sizeof...(Args) == 0) {
    static_assert(!std::is_same_v<T, void>,
                  "Sequential with no transforms requires explicit type");
    return transform::Sequential<T>(
        std::vector<std::unique_ptr<transform::ITransform<T, T>>>{}
        );
  } else {
    using DeducedT = typename std::tuple_element_t<
      0, std::tuple<Args...>>::element_type::input_type;
    std::vector<std::unique_ptr<ITransform<DeducedT, DeducedT>>> transforms;
    (transforms.push_back(std::forward<Args>(args)), ...);
    return std::make_unique<Sequential<DeducedT>>(std::move(transforms));
  }
}
}

#endif //SPAM_CLASSIFIER_COMPOSE_H