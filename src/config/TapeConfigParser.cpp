#include "config/TapeConfigParser.hpp"

#include <yaml-cpp/yaml.h>

#include <stdexcept>

namespace ext_sort::config {

namespace {

void ApplyTapeSection(const YAML::Node& tape, TapeConfig& config) {
  if (auto val = tape["read_delay_ms"]) {
    config.read_delay_ms = val.as<int32_t>();
  }

  if (auto val = tape["write_delay_ms"]) {
    config.write_delay_ms = val.as<int32_t>();
  }

  if (auto val = tape["rewind_delay_ms"]) {
    config.rewind_delay_ms = val.as<int32_t>();
  }

  if (auto val = tape["shift_delay_ms"]) {
    config.shift_delay_ms = val.as<int32_t>();
  }
}

void ApplySorterSection(const YAML::Node& sorter, TapeConfig& config) {
  if (auto val = sorter["ram_limit_bytes"]) {
    config.ram_limit_bytes = val.as<int64_t>();
  }

  if (auto val = sorter["tmp_dir"]) {
    config.tmp_dir = val.as<std::string>();
  }
}

}  // namespace

TapeConfig TapeConfigParser::Parse(const std::string& path) try {
  YAML::Node root = YAML::LoadFile(path);
  TapeConfig config;
  if (auto tape = root["tape"]) {
    ApplyTapeSection(tape, config);
  }
  if (auto sorter = root["sorter"]) {
    ApplySorterSection(sorter, config);
  }
  return config;
} catch (const YAML::Exception& exc) {
  throw std::runtime_error(std::string("Failed to load config: ") + exc.what());
}

}  // namespace ext_sort::config