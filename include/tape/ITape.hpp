#pragma once

#include <cstdint>

#include "tape/TapeException.hpp"

namespace ext_sort::tape {

class ITape {
 public:
  ITape() = default;
  virtual ~ITape() = default;

  ITape(const ITape&) = delete;
  ITape& operator=(const ITape&) = delete;
  ITape(ITape&&) = default;
  ITape& operator=(ITape&&) = default;

  [[nodiscard]] virtual int32_t Read() = 0;
  virtual void Write(int32_t value) = 0;
  virtual void MoveForward() = 0;
  virtual void MoveBackward() = 0;
  virtual void Rewind() = 0;
  [[nodiscard]] virtual size_t Size() const = 0;
};

}  // namespace ext_sort::tape
