#pragma once

#include <cstddef>
#include <cstdint>
#include <iterator>

#include "tape/ITape.hpp"

namespace ext_sort::tape {

struct TapeInputSentinel {
  size_t pos = 0;
};

class TapeInputIterator {
 public:
  // NOLINTBEGIN(readability-identifier-naming)
  using iterator_category = std::input_iterator_tag;
  using value_type = int32_t;
  using difference_type = std::ptrdiff_t;
  using pointer = const int32_t*;
  using reference = int32_t;
  // NOLINTEND(readability-identifier-naming)

  TapeInputIterator(ITape* tape, size_t pos, size_t end_pos)
      : tape_(tape), pos_(pos), end_pos_(end_pos) {}

  int32_t operator*() const { return tape_->Read(); }

  TapeInputIterator& operator++() {
    if (pos_ + 1 < end_pos_) {
      tape_->MoveForward();
    }
    ++pos_;
    return *this;
  }

  TapeInputIterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  bool operator==(const TapeInputIterator& other) const {
    return pos_ == other.pos_;
  }

  bool operator==(const TapeInputSentinel& sentinel) const {
    return pos_ == sentinel.pos;
  }

 private:
  ITape* tape_;
  size_t pos_;
  size_t end_pos_;
};

class TapeInputView {
 public:
  TapeInputView(ITape& tape, size_t count) : tape_(&tape), count_(count) {}

  // NOLINTBEGIN(readability-identifier-naming)
  [[nodiscard]] TapeInputIterator begin() const { return {tape_, 0, count_}; }

  [[nodiscard]] TapeInputSentinel end() const { return {count_}; }
  // NOLINTEND(readability-identifier-naming)

 private:
  ITape* tape_;
  size_t count_;
};

class TapeOutputIterator {
 public:
  // NOLINTBEGIN(readability-identifier-naming)
  using iterator_category = std::output_iterator_tag;
  using value_type = void;
  using difference_type = std::ptrdiff_t;
  using pointer = void;
  using reference = void;
  // NOLINTEND(readability-identifier-naming)

  explicit TapeOutputIterator(ITape& tape) : tape_(&tape), size_(tape.Size()) {}

  TapeOutputIterator& operator=(int32_t value) {
    tape_->Write(value);
    return *this;
  }

  TapeOutputIterator& operator*() { return *this; }

  TapeOutputIterator& operator++() {
    if (pos_ + 1 < size_) {
      tape_->MoveForward();
    }
    ++pos_;
    return *this;
  }

  TapeOutputIterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

 private:
  ITape* tape_;
  size_t pos_ = 0;
  size_t size_ = 0;
};

static_assert(std::input_iterator<TapeInputIterator>);
static_assert(std::output_iterator<TapeOutputIterator, int32_t>);

}  // namespace ext_sort::tape
