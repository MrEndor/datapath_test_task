#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "tape/ITape.hpp"

namespace ext_sort::tape {

class MemoryTape : public ITape {
 public:
  explicit MemoryTape(size_t element_count)
      : data_(element_count, 0), pos_(0) {}

  [[nodiscard]] int32_t Read() override {
    if (pos_ >= data_.size()) {
      throw EndOfTapeException();
    }
    return data_[pos_];
  }

  void Write(int32_t value) override {
    if (pos_ >= data_.size()) {
      throw EndOfTapeException();
    }
    data_[pos_] = value;
  }

  void MoveForward() override {
    if (pos_ + 1 >= data_.size()) {
      throw EndOfTapeException();
    }
    ++pos_;
  }

  void MoveBackward() override {
    if (pos_ == 0) {
      throw BeginOfTapeException();
    }
    --pos_;
  }

  void Rewind() override { pos_ = 0; }

  [[nodiscard]] size_t Size() const override { return data_.size(); }

  [[nodiscard]] const std::vector<int32_t>& Data() const { return data_; }

 private:
  std::vector<int32_t> data_;
  size_t pos_;
};

}  // namespace ext_sort::tape
