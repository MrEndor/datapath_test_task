#include <gtest/gtest.h>

#include <filesystem>
#include <limits>
#include <vector>

#include "tape/FileTape.hpp"
#include "tape/FileTapeFactory.hpp"
#include "tape/TapeException.hpp"

using namespace ext_sort::config;
using namespace ext_sort::tape;

namespace {

std::string TmpPath(const std::string& name) {
  return (std::filesystem::temp_directory_path() / name).string();
}

TapeConfig ZeroDelayConfig() {
  TapeConfig cfg;
  cfg.tmp_dir = std::filesystem::temp_directory_path().string();
  return cfg;
}

constexpr int32_t kVal42 = 42;
constexpr int32_t kVal100 = 100;
constexpr int32_t kVal200 = 200;
constexpr int32_t kVal7 = 7;
constexpr int32_t kVal3 = 3;
constexpr size_t kSize2 = 2;
constexpr size_t kSize3 = 3;
constexpr size_t kSize5 = 5;

}  // namespace

TEST(FileTape, WriteAndRead_SingleElement) {
  auto path = TmpPath("ft_single.bin");
  {
    FileTape tape(path, 1U, ZeroDelayConfig());
    tape.Write(kVal42);
    EXPECT_EQ(tape.Read(), kVal42);
  }
  std::filesystem::remove(path);
}

TEST(FileTape, WriteAndRead_MultipleElements) {
  auto path = TmpPath("ft_multi.bin");
  const std::vector kData = {1, -1, std::numeric_limits<int32_t>::max(),
                             std::numeric_limits<int32_t>::min()};
  {
    FileTape tape(path, kData.size(), ZeroDelayConfig());
    for (size_t idx = 0; idx < kData.size(); ++idx) {
      tape.Write(kData[idx]);
      if (idx + 1 < kData.size()) {
        tape.MoveForward();
      }
    }
    tape.Rewind();
    for (size_t idx = 0; idx < kData.size(); ++idx) {
      EXPECT_EQ(tape.Read(), kData[idx]);
      if (idx + 1 < kData.size()) {
        tape.MoveForward();
      }
    }
  }
  std::filesystem::remove(path);
}

TEST(FileTape, MoveForward_AtEnd_Throws) {
  auto path = TmpPath("ft_eof.bin");
  {
    FileTape tape(path, kSize2, ZeroDelayConfig());
    EXPECT_NO_THROW(tape.MoveForward());
    EXPECT_THROW(tape.MoveForward(), EndOfTapeException);
  }
  std::filesystem::remove(path);
}

TEST(FileTape, MoveBackward_AtStart_Throws) {
  auto path = TmpPath("ft_back.bin");
  {
    FileTape tape(path, 1U, ZeroDelayConfig());
    EXPECT_THROW(tape.MoveBackward(), BeginOfTapeException);
  }
  std::filesystem::remove(path);
}

TEST(FileTape, Read_EmptyTape_Throws) {
  auto path = TmpPath("ft_empty.bin");
  {
    FileTape tape(path, 0U, ZeroDelayConfig());
    EXPECT_THROW((void)tape.Read(), EndOfTapeException);
  }
  std::filesystem::remove(path);
}

TEST(FileTape, Rewind_ThenReadFromStart) {
  auto path = TmpPath("ft_rewind.bin");
  {
    FileTape tape(path, kSize3, ZeroDelayConfig());
    tape.Write(1);
    tape.MoveForward();
    tape.Write(2);
    tape.MoveForward();
    tape.Write(kVal3);
    tape.Rewind();
    EXPECT_EQ(tape.Read(), 1);
  }
  std::filesystem::remove(path);
}

TEST(FileTape, OpenExistingFile_ReadsCorrectly) {
  auto path = TmpPath("ft_existing.bin");
  {
    FileTape tape(path, kSize2, ZeroDelayConfig());
    tape.Write(kVal100);
    tape.MoveForward();
    tape.Write(kVal200);
  }
  {
    FileTape tape(path, ZeroDelayConfig());
    EXPECT_EQ(tape.Size(), kSize2);
    EXPECT_EQ(tape.Read(), kVal100);
  }
  std::filesystem::remove(path);
}

TEST(FileTapeFactory, CreatesWorkingTape) {
  TapeConfig cfg = ZeroDelayConfig();
  cfg.tmp_dir =
      (std::filesystem::temp_directory_path() / "tape_factory_test").string();
  FileTapeFactory factory(cfg);
  auto tape = factory.Create(kSize5);
  EXPECT_EQ(tape->Size(), kSize5);
  tape->Write(kVal7);
  EXPECT_EQ(tape->Read(), kVal7);
}

TEST(FileTapeFactory, MultipleCreates_IndependentTapes) {
  TapeConfig cfg = ZeroDelayConfig();
  cfg.tmp_dir =
      (std::filesystem::temp_directory_path() / "tape_factory_multi").string();
  FileTapeFactory factory(cfg);
  auto tape1 = factory.Create(kSize3);
  auto tape2 = factory.Create(kSize3);
  tape1->Write(1);
  tape2->Write(2);
  EXPECT_EQ(tape1->Read(), 1);
  EXPECT_EQ(tape2->Read(), 2);
}

TEST(FileTapeFactory, RaiiDeletesTempFiles) {
  TapeConfig cfg = ZeroDelayConfig();
  cfg.tmp_dir =
      (std::filesystem::temp_directory_path() / "tape_factory_raii").string();
  std::string tape_path;
  {
    FileTapeFactory factory(cfg);
    (void)factory.Create(1U);
    tape_path = cfg.tmp_dir + "/tape_000000.bin";
    ASSERT_TRUE(std::filesystem::exists(tape_path));
  }
  EXPECT_FALSE(std::filesystem::exists(tape_path));
}
