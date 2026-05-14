#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "tape/ITape.hpp"
#include "tape/ITapeFactory.hpp"

namespace ext_sort::sorting {

class TapeSorter {
 public:
  TapeSorter(tape::ITapeFactory& factory, int64_t ram_limit_bytes);
  void Sort(tape::ITape& input, tape::ITape& output);

 private:
  using TapePtr = std::unique_ptr<tape::ITape>;

  std::vector<TapePtr> SortToTapes(tape::ITape& input, size_t tape_capacity);
  void MergeTapes(std::vector<TapePtr>& sorted_tapes,
                  tape::ITape& output) const;
  [[nodiscard]] size_t MaxMergeWays(size_t tape_count) const;
  static void KWayMerge(std::vector<TapePtr>& tapes_to_merge,
                        tape::ITape& dest);

  tape::ITapeFactory& factory_;
  int64_t ram_limit_bytes_;
};

}  // namespace ext_sort::sorting
