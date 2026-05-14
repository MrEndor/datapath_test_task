#pragma once

#include <cstddef>
#include <memory>

#include "tape/ITape.hpp"

namespace ext_sort::tape {

class ITapeFactory {
 public:
  virtual ~ITapeFactory() = default;
  [[nodiscard]] virtual std::unique_ptr<ITape> Create(size_t element_count) = 0;
};

}  // namespace ext_sort::tape
