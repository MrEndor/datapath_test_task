#include <gtest/gtest.h>

#include <algorithm>
#include <numeric>
#include <vector>

#include "sorting/TapeSorter.hpp"
#include "support/MemoryTape.hpp"
#include "support/MemoryTapeFactory.hpp"

using namespace ext_sort::tape;
using namespace ext_sort::sorting;

namespace {

MemoryTape MakeTape(const std::vector<int32_t>& vals) {
  MemoryTape tape(vals.size());
  for (size_t idx = 0; idx < vals.size(); ++idx) {
    tape.Write(vals[idx]);
    if (idx + 1 < vals.size()) {
      tape.MoveForward();
    }
  }
  return tape;
}

std::vector<int32_t> ReadAll(ITape& tape) {
  if (tape.Size() == 0) {
    return {};
  }

  tape.Rewind();
  std::vector<int32_t> result;
  result.reserve(tape.Size());
  for (size_t idx = 0; idx < tape.Size(); ++idx) {
    result.push_back(tape.Read());
    if (idx + 1 < tape.Size()) {
      tape.MoveForward();
    }
  }
  return result;
}

void SortAndCheck(const std::vector<int32_t>& input, int64_t ram_limit_bytes) {
  MemoryTapeFactory factory;

  auto in_tape = MakeTape(input);
  MemoryTape out_tape(input.size());
  TapeSorter sorter(factory, ram_limit_bytes);

  sorter.Sort(in_tape, out_tape);
  auto expected = input;
  std::ranges::sort(expected);

  EXPECT_EQ(ReadAll(out_tape), expected);
}

constexpr int64_t kMem64 = 64;
constexpr int64_t kMem1024 = 1024;
constexpr int64_t kMem12 = 12;
constexpr int64_t kMem8 = 8;

}  // namespace

TEST(TapeSorter, EmptyInput_OutputEmpty) {
  MemoryTapeFactory factory;
  MemoryTape in_tape(0);
  MemoryTape out_tape(0);
  TapeSorter sorter(factory, kMem64);

  EXPECT_NO_THROW(sorter.Sort(in_tape, out_tape));
  EXPECT_EQ(out_tape.Size(), 0U);
}

TEST(TapeSorter, SingleElement) { SortAndCheck({42}, kMem64); }

TEST(TapeSorter, AlreadySorted_FitsInMemory) {
  SortAndCheck({1, 2, 3, 4, 5}, kMem1024);
}

TEST(TapeSorter, ReverseOrder_FitsInMemory) {
  SortAndCheck({5, 4, 3, 2, 1}, kMem1024);
}

TEST(TapeSorter, Duplicates_FitsInMemory) {
  SortAndCheck({3, 1, 2, 1, 3, 2}, kMem1024);
}

TEST(TapeSorter, AllEqual_FitsInMemory) {
  SortAndCheck({7, 7, 7, 7, 7}, kMem1024);
}

TEST(TapeSorter, NegativeValues_FitsInMemory) {
  SortAndCheck({-5, 3, -1, 0, 2, -3}, kMem1024);
}

TEST(TapeSorter, ExactlyTapeCapacity_FastPath) {
  SortAndCheck({3, 1, 2}, kMem12);
}

TEST(TapeSorter, TapeCapacityPlusOne_MultipleTapes) {
  SortAndCheck({4, 3, 1, 2}, kMem12);
}

TEST(TapeSorter, MemoryTape_MoveBackwardAtBegin_Throws) {
  MemoryTape tape(3);
  EXPECT_THROW(tape.MoveBackward(), BeginOfTapeException);
}

TEST(TapeSorter, MultipleTapes_SingleMergePass) {
  SortAndCheck({9, 1, 5, 3, 7, 2, 8, 4, 6}, kMem12);
}

TEST(TapeSorter, MultipleTapes_MultipleMergePasses) {
  SortAndCheck({10, 9, 8, 7, 6, 5, 4, 3, 2, 1}, kMem8);
}

TEST(TapeSorter, LargerDataset) {
  std::vector<int32_t> data(10000);
  std::iota(data.begin(), data.end(), -50);
  std::ranges::reverse(data);
  SortAndCheck(data, kMem64);
}
