#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "config/TapeConfigParser.hpp"

using namespace ext_sort::config;

namespace {

std::string WriteTempYaml(const std::string& content) {
  auto path =
      (std::filesystem::temp_directory_path() / "test_config.yaml").string();
  std::ofstream file(path);
  file << content;
  return path;
}

}  // namespace

TEST(TapeConfigParser, FullConfig) {
  auto path = WriteTempYaml(R"(
tape:
  read_delay_ms: 5
  write_delay_ms: 10
  rewind_delay_ms: 100
  shift_delay_ms: 2
sorter:
  ram_limit_bytes: 1048576
  tmp_dir: /tmp/tapes
)");
  auto cfg = TapeConfigParser::Parse(path);

  EXPECT_EQ(cfg.read_delay_ms, 5);
  EXPECT_EQ(cfg.write_delay_ms, 10);
  EXPECT_EQ(cfg.rewind_delay_ms, 100);
  EXPECT_EQ(cfg.shift_delay_ms, 2);
  EXPECT_EQ(cfg.ram_limit_bytes, 1048576);
  EXPECT_EQ(cfg.tmp_dir, "/tmp/tapes");
}

TEST(TapeConfigParser, PartialConfig_UsesDefaults) {
  auto path = WriteTempYaml("tape:\n  read_delay_ms: 3\n");
  auto cfg = TapeConfigParser::Parse(path);

  EXPECT_EQ(cfg.read_delay_ms, 3);
  EXPECT_EQ(cfg.write_delay_ms, 0);
  EXPECT_EQ(cfg.ram_limit_bytes, kDefaultRamLimitBytes);
  EXPECT_EQ(cfg.tmp_dir, "./tmp");
}

TEST(TapeConfigParser, EmptyConfig_AllDefaults) {
  auto path = WriteTempYaml("");
  auto cfg = TapeConfigParser::Parse(path);

  EXPECT_EQ(cfg.read_delay_ms, 0);
  EXPECT_EQ(cfg.tmp_dir, "./tmp");
}

TEST(TapeConfigParser, MissingFile_Throws) {
  EXPECT_THROW(TapeConfigParser::Parse("/no/such/file.yaml"),
               std::runtime_error);
}

TEST(TapeConfigParser, UnknownKeys_Ignored) {
  auto path = WriteTempYaml("unknown_key: 42\ntape:\n  read_delay_ms: 1\n");

  EXPECT_NO_THROW(TapeConfigParser::Parse(path));
}
