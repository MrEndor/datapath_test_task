#pragma once

#include <string>

#include "config/TapeConfig.hpp"

namespace ext_sort::config {

class TapeConfigParser {
 public:
  static TapeConfig Parse(const std::string& path);
};

}  // namespace ext_sort::config