#pragma once

#include <cstdint>
#include <string>

namespace ext_sort::config {

inline constexpr int64_t kDefaultRamLimitBytes = 64LL * 1024 * 1024;

struct TapeConfig {
  int32_t read_delay_ms = 0;
  int32_t write_delay_ms = 0;
  int32_t rewind_delay_ms = 0;
  int32_t shift_delay_ms = 0;
  int64_t ram_limit_bytes = kDefaultRamLimitBytes;
  std::string tmp_dir = "./tmp";
};

}  // namespace ext_sort::config
