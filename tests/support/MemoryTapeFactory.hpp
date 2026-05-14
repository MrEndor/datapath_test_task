#pragma once

#include <memory>

#include "support/MemoryTape.hpp"
#include "tape/ITapeFactory.hpp"

namespace ext_sort::tape {

class MemoryTapeFactory : public ITapeFactory {
 public:
  [[nodiscard]] std::unique_ptr<ITape> Create(size_t element_count) override {
    return std::make_unique<MemoryTape>(element_count);
  }
};

}  // namespace ext_sort::tape
