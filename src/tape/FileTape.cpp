#include "tape/FileTape.hpp"

#include <chrono>
#include <thread>
#include <vector>

#include "tape/TapeException.hpp"

namespace ext_sort::tape {

namespace {

constexpr int64_t kBytesPerElement = sizeof(int32_t);

void SleepMs(int32_t millis) {
  if (millis > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
  }
}

}  // namespace

FileTape::FileTape(const std::string& path, const config::TapeConfig& config)
    : config_(config) {
  file_.open(path, std::ios::in | std::ios::out | std::ios::binary);
  if (!file_.is_open()) {
    throw TapeIoException("cannot open tape file: " + path);
  }

  file_.seekg(0, std::ios::end);
  const auto kBytes = static_cast<int64_t>(file_.tellg());
  if (kBytes % kBytesPerElement != 0) {
    throw TapeIoException("tape file size not aligned: " + path);
  }
  size_ = static_cast<size_t>(kBytes / kBytesPerElement);
  file_.seekg(0);
}

FileTape::FileTape(const std::string& path, size_t element_count,
                   const config::TapeConfig& config)
    : size_(element_count), config_(config) {
  {
    std::ofstream out(path, std::ios::binary | std::ios::trunc);
    if (!out.is_open()) {
      throw TapeIoException("cannot create tape file: " + path);
    }
    if (element_count > 0) {
      std::vector<int32_t> zeros(element_count, 0);
      out.write(reinterpret_cast<const char*>(zeros.data()),
                static_cast<std::streamsize>(element_count) * kBytesPerElement);
    }
  }

  file_.open(path, std::ios::in | std::ios::out | std::ios::binary);
  if (!file_.is_open()) {
    throw TapeIoException("cannot reopen tape file: " + path);
  }
}

FileTape::~FileTape() { FlushChunk(); }

bool FileTape::InBuffer(size_t pos) const {
  return buf_loaded_ && pos >= buf_start_ && pos < buf_start_ + kBufSize;
}

void FileTape::FlushChunk() {
  if (!buf_dirty_ || !buf_loaded_) {
    return;
  }

  const size_t kCount = std::min(kBufSize, size_ - buf_start_);
  file_.seekp(static_cast<std::streamoff>(buf_start_) * kBytesPerElement);
  file_.write(reinterpret_cast<const char*>(buf_.data()),
              static_cast<std::streamsize>(kCount) * kBytesPerElement);

  buf_dirty_ = false;
}

void FileTape::LoadChunk(size_t pos) {
  FlushChunk();

  buf_start_ = (pos / kBufSize) * kBufSize;
  const size_t kCount = std::min(kBufSize, size_ - buf_start_);

  buf_.fill(0);
  file_.seekg(static_cast<std::streamoff>(buf_start_) * kBytesPerElement);
  file_.read(reinterpret_cast<char*>(buf_.data()),
             static_cast<std::streamsize>(kCount) * kBytesPerElement);

  buf_loaded_ = true;
}

int32_t FileTape::Read() {
  if (pos_ >= size_) {
    throw EndOfTapeException();
  }

  SleepMs(config_.read_delay_ms);

  if (!InBuffer(pos_)) {
    LoadChunk(pos_);
  }
  return buf_[pos_ - buf_start_];
}

void FileTape::Write(int32_t value) {
  if (pos_ >= size_) {
    throw EndOfTapeException();
  }

  SleepMs(config_.write_delay_ms);

  if (!InBuffer(pos_)) {
    LoadChunk(pos_);
  }
  buf_[pos_ - buf_start_] = value;
  buf_dirty_ = true;
}

void FileTape::MoveForward() {
  if (pos_ + 1 >= size_) {
    throw EndOfTapeException();
  }
  SleepMs(config_.shift_delay_ms);
  ++pos_;
}

void FileTape::MoveBackward() {
  if (pos_ == 0) {
    throw BeginOfTapeException();
  }
  SleepMs(config_.shift_delay_ms);
  --pos_;
}

void FileTape::Rewind() {
  SleepMs(config_.rewind_delay_ms);
  FlushChunk();
  pos_ = 0;
}

}  // namespace ext_sort::tape
