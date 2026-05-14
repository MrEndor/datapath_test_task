#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

#include "config/TapeConfigParser.hpp"
#include "sorting/TapeSorter.hpp"
#include "tape/FileTape.hpp"
#include "tape/FileTapeFactory.hpp"

namespace {

constexpr int64_t kKilobyte = 1024;
constexpr int64_t kMegabyte = 1024 * 1024;
constexpr int64_t kGigabyte = 1024LL * 1024 * 1024;

int64_t ParseMemory(const std::string& str) {
  if (str.empty()) {
    throw std::runtime_error("empty --memory value");
  }
  const char kSuffix =
      static_cast<char>(std::toupper(static_cast<unsigned char>(str.back())));
  const int64_t kBase = std::stoll(str);
  if (kSuffix == 'K') {
    return kBase * kKilobyte;
  }
  if (kSuffix == 'M') {
    return kBase * kMegabyte;
  }
  if (kSuffix == 'G') {
    return kBase * kGigabyte;
  }
  return kBase;
}

}  // namespace

int main(int argc, char** argv) {
  std::string config_path = "./config.yaml";
  std::string memory_str;
  std::string input_path;
  std::string output_path;

  CLI::App cli{"External K-way merge sort over tape"};
  cli.add_option("--config,-c", config_path, "YAML config file")
      ->capture_default_str();
  cli.add_option("--memory,-m", memory_str,
                 "memory limit (e.g. 64M, 256K, 1G, or raw bytes)");
  cli.add_option("input", input_path, "input tape file")->required();
  cli.add_option("output", output_path, "output tape file")->required();

  try {
    cli.parse(argc, argv);
  } catch (const CLI::ParseError& exc) {
    return cli.exit(exc);
  }
  try {
    using namespace ext_sort;

    auto config = config::TapeConfigParser::Parse(config_path);
    if (!memory_str.empty()) {
      config.ram_limit_bytes = ParseMemory(memory_str);
    }

    const size_t kElemCount =
        std::filesystem::file_size(input_path) / sizeof(int32_t);

    tape::FileTape input(input_path, config);
    tape::FileTape output(output_path, kElemCount, config);
    tape::FileTapeFactory factory(config);

    sorting::TapeSorter(factory, config.ram_limit_bytes).Sort(input, output);
  } catch (const std::exception& exc) {
    std::cerr << "Error: " << exc.what() << '\n';
    return 1;
  }
}
