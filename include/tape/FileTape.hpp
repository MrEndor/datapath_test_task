#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

#include "config/TapeConfig.hpp"
#include "tape/ITape.hpp"

namespace ext_sort::tape {

class FileTape : public ITape {
 public:
  FileTape(const std::string& path, const config::TapeConfig& config);
  FileTape(const std::string& path, size_t element_count,
           const config::TapeConfig& config);

  ~FileTape() override;

  [[nodiscard]] size_t Size() const override { return size_; }

  [[nodiscard]] int32_t Read() override;
  void Write(int32_t value) override;
  void MoveForward() override;
  void MoveBackward() override;
  void Rewind() override;

 private:
  static constexpr size_t kBufSize = 4096;

  void LoadChunk(size_t pos);
  void FlushChunk();
  [[nodiscard]] bool InBuffer(size_t pos) const;

  std::fstream file_;
  size_t size_ = 0;
  size_t pos_ = 0;
  config::TapeConfig config_;

  std::array<int32_t, kBufSize> buf_{};
  size_t buf_start_ = 0;
  bool buf_loaded_ = false;
  bool buf_dirty_ = false;
};

}  // namespace ext_sort::tape
