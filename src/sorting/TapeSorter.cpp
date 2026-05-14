#include "sorting/TapeSorter.hpp"

#include <sys/resource.h>

#include <algorithm>
#include <queue>
#include <utility>

#include "tape/TapeIterators.hpp"

namespace {

constexpr int64_t kFdReserve = 32;
constexpr int64_t kFdLimitFallback = 1024;

int64_t FdSoftLimit() {
  rlimit fd_rlimit{};
  if (getrlimit(RLIMIT_NOFILE, &fd_rlimit) == 0) {
    return static_cast<int64_t>(fd_rlimit.rlim_cur);
  }
  return kFdLimitFallback;
}

}  // namespace

namespace ext_sort::sorting {

TapeSorter::TapeSorter(tape::ITapeFactory& factory, int64_t ram_limit_bytes)
    : factory_(factory), ram_limit_bytes_(ram_limit_bytes) {}

void TapeSorter::Sort(tape::ITape& input, tape::ITape& output) {
  if (input.Size() == 0) {
    return;
  }

  const size_t kTapeCapacity =
      static_cast<size_t>(ram_limit_bytes_) / sizeof(int32_t);

  if (input.Size() <= kTapeCapacity) {
    std::vector<int32_t> buf(input.Size());
    input.Rewind();
    std::ranges::copy(tape::TapeInputView(input, input.Size()), buf.begin());
    std::ranges::sort(buf);
    output.Rewind();
    std::ranges::copy(buf, tape::TapeOutputIterator(output));
    return;
  }

  auto sorted_tapes = SortToTapes(input, kTapeCapacity);
  MergeTapes(sorted_tapes, output);
}

std::vector<TapeSorter::TapePtr> TapeSorter::SortToTapes(
    tape::ITape& input, size_t tape_capacity) {
  std::vector<TapePtr> sorted_tapes;
  std::vector<int32_t> buf;
  buf.reserve(tape_capacity);

  input.Rewind();
  size_t remaining = input.Size();

  while (remaining > 0) {
    const size_t kTapeSize = std::min(tape_capacity, remaining);
    buf.resize(kTapeSize);

    std::ranges::copy(tape::TapeInputView(input, kTapeSize), buf.begin());
    std::ranges::sort(buf);

    auto tape = factory_.Create(kTapeSize);
    std::ranges::copy(buf, tape::TapeOutputIterator(*tape));

    sorted_tapes.push_back(std::move(tape));
    remaining -= kTapeSize;

    if (remaining > 0) {
      input.MoveForward();
    }
  }

  return sorted_tapes;
}

size_t TapeSorter::MaxMergeWays(size_t tape_count) const {
  const int64_t kRamWays =
      static_cast<int64_t>(static_cast<size_t>(ram_limit_bytes_) /
                           sizeof(int32_t)) -
      1;
  const int64_t kFdWays = FdSoftLimit() - kFdReserve;
  const int64_t kWays =
      std::max(int64_t{2}, std::min({static_cast<int64_t>(tape_count),
                                     kRamWays, kFdWays}));
  return static_cast<size_t>(kWays);
}

void TapeSorter::MergeTapes(std::vector<TapePtr>& sorted_tapes,
                            tape::ITape& output) const {
  while (sorted_tapes.size() > 1) {
    const size_t kMergeWidth = MaxMergeWays(sorted_tapes.size());

    const bool kIsFinalPass = sorted_tapes.size() <= kMergeWidth;
    std::vector<TapePtr> next;

    for (size_t idx = 0; idx < sorted_tapes.size(); idx += kMergeWidth) {
      const size_t kEnd = std::min(idx + kMergeWidth, sorted_tapes.size());
      std::vector<TapePtr> tapes_to_merge;
      size_t total = 0;

      for (size_t jdx = idx; jdx < kEnd; ++jdx) {
        total += sorted_tapes[jdx]->Size();
        tapes_to_merge.emplace_back(std::move(sorted_tapes[jdx]));
      }

      if (tapes_to_merge.size() == 1) {
        next.emplace_back(std::move(tapes_to_merge[0]));
        continue;
      }

      if (kIsFinalPass) {
        KWayMerge(tapes_to_merge, output);
        return;
      }

      auto dest = factory_.Create(total);
      KWayMerge(tapes_to_merge, *dest);
      next.emplace_back(std::move(dest));
    }

    sorted_tapes = std::move(next);
  }
}

void TapeSorter::KWayMerge(std::vector<TapePtr>& tapes_to_merge,
                           tape::ITape& dest) {
  using Entry = std::pair<int32_t, size_t>;
  std::priority_queue<Entry, std::vector<Entry>, std::greater<Entry>> min_heap;
  std::vector<size_t> pos(tapes_to_merge.size(), 0);

  for (size_t idx = 0; idx < tapes_to_merge.size(); ++idx) {
    tapes_to_merge[idx]->Rewind();
    if (tapes_to_merge[idx]->Size() > 0) {
      min_heap.push({tapes_to_merge[idx]->Read(), idx});
    }
  }

  size_t written = 0;
  const size_t kTotal = dest.Size();
  dest.Rewind();

  while (!min_heap.empty()) {
    auto [val, gidx] = min_heap.top();
    min_heap.pop();
    dest.Write(val);
    ++written;

    if (written < kTotal) {
      dest.MoveForward();
    }

    ++pos[gidx];
    if (pos[gidx] < tapes_to_merge[gidx]->Size()) {
      tapes_to_merge[gidx]->MoveForward();
      min_heap.push({tapes_to_merge[gidx]->Read(), gidx});
    }
  }
}

}  // namespace ext_sort::sorting
