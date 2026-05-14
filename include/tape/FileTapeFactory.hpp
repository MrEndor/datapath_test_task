#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "config/TapeConfig.hpp"
#include "tape/FileTape.hpp"
#include "tape/ITapeFactory.hpp"

namespace ext_sort::tape {

class FileTapeFactory : public ITapeFactory {
 public:
  explicit FileTapeFactory(const config::TapeConfig& config) : config_(config) {
    std::filesystem::create_directories(config_.tmp_dir);
  }

  ~FileTapeFactory() override {
    for (const auto& path : temp_file_paths_) {
      std::error_code err;
      std::filesystem::remove(path, err);
    }
  }

  [[nodiscard]] std::unique_ptr<ITape> Create(size_t element_count) override {
    auto path =
        std::format("{}/tape_{:06d}.bin", config_.tmp_dir, next_tape_id_++);
    temp_file_paths_.push_back(path);

    return std::make_unique<FileTape>(path, element_count, config_);
  }

 private:
  config::TapeConfig config_;
  std::vector<std::string> temp_file_paths_;
  int next_tape_id_ = 0;
};

}  // namespace ext_sort::tape
