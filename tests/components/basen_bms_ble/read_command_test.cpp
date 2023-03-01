#include <gtest/gtest.h>
#include <vector>
#include "common.h"

namespace esphome::basen_bms_ble::testing {

// Read/poll frame layout (SOF=0x3A or 0x3B, data_len=1):
//   [0]      SOF         0x3A (poll) or 0x3B (initial connect)
//   [1]      address     0x16
//   [2]      function    see below
//   [3]      data_len    0x01
//   [4]      value       0x00
//   [5]      CRC low
//   [6]      CRC high
//   [7]      EOF         0x0D
//   [8]      EOF         0x0A
//
// CRC = sum(frame[1..4]) (address through value)
//
// Physical BLE captures (btsnoop_hci_commands.txt):
//   Cell voltages 1-12  (func=0x24): 3a 16 24 01 00 3b 00 0d 0a
//   Cell voltages 13-24 (func=0x25): 3a 16 25 01 00 3c 00 0d 0a
//   Status poll         (func=0x2A): 3a 16 2a 01 00 41 00 0d 0a
//   General info        (func=0x2B): 3a 16 2b 01 00 42 00 0d 0a
//   Balancing           (func=0xFE): 3a 16 fe 01 00 15 01 0d 0a
//   Initial connect     (func=0x2A): 3b 16 2a 01 00 41 00 0d 0a

TEST(ReadCommandTest, CellVoltages1To12) {
  TestableBasenBmsBle bms;
  const uint8_t data[1] = {0x00};
  auto f = bms.build_frame_(0x3A, 0x24, data, 1);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3A, 0x16, 0x24, 0x01, 0x00, 0x3B, 0x00, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(ReadCommandTest, CellVoltages13To24) {
  TestableBasenBmsBle bms;
  const uint8_t data[1] = {0x00};
  auto f = bms.build_frame_(0x3A, 0x25, data, 1);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3A, 0x16, 0x25, 0x01, 0x00, 0x3C, 0x00, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(ReadCommandTest, StatusPoll) {
  TestableBasenBmsBle bms;
  const uint8_t data[1] = {0x00};
  auto f = bms.build_frame_(0x3A, 0x2A, data, 1);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3A, 0x16, 0x2A, 0x01, 0x00, 0x41, 0x00, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(ReadCommandTest, GeneralInfo) {
  TestableBasenBmsBle bms;
  const uint8_t data[1] = {0x00};
  auto f = bms.build_frame_(0x3A, 0x2B, data, 1);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3A, 0x16, 0x2B, 0x01, 0x00, 0x42, 0x00, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(ReadCommandTest, Balancing) {
  TestableBasenBmsBle bms;
  const uint8_t data[1] = {0x00};
  auto f = bms.build_frame_(0x3A, 0xFE, data, 1);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3A, 0x16, 0xFE, 0x01, 0x00, 0x15, 0x01, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(ReadCommandTest, InitialConnect) {
  TestableBasenBmsBle bms;
  const uint8_t data[1] = {0x00};
  auto f = bms.build_frame_(0x3B, 0x2A, data, 1);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3B, 0x16, 0x2A, 0x01, 0x00, 0x41, 0x00, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

}  // namespace esphome::basen_bms_ble::testing
