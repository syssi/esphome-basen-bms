#include <gtest/gtest.h>
#include <vector>
#include "common.h"

namespace esphome::basen_bms_ble::testing {

// Write frame layout (SOF=0x3B, func=0xEB, data_len=4):
//   [0]      SOF         0x3B
//   [1]      address     0x16
//   [2]      function    0xEB
//   [3]      data_len    0x04
//   [4]      sub_cmd     0x86
//   [5]      reg_low     0x1D  (register 0x011D low byte)
//   [6]      reg_high    0x01  (register 0x011D high byte)
//   [7]      mos_state   bitmask: bit0=charging, bit1=discharging
//   [8]      CRC low
//   [9]      CRC high
//   [10]     EOF         0x0D
//   [11]     EOF         0x0A
//
// CRC = sum(frame[1..7]) (address through mos_state)
//
// Physical BLE captures (btsnoop_hci_commands.txt):
//   Both OFF  (0x00): 3b 16 eb 04 86 1d 01 00 a9 01 0d 0a
//   Chg ON    (0x01): 3b 16 eb 04 86 1d 01 01 aa 01 0d 0a
//   Dsg ON    (0x02): 3b 16 eb 04 86 1d 01 02 ab 01 0d 0a
//   Both ON   (0x03): 3b 16 eb 04 86 1d 01 03 ac 01 0d 0a

// ── Frame header / trailer ────────────────────────────────────────────────────

TEST(WriteCommandTest, HeaderAndTrailer) {
  TestableBasenBmsBle bms;
  const uint8_t data[4] = {0x86, 0x1D, 0x01, 0x00};
  auto f = bms.build_frame_(0x3B, 0xEB, data, 4);

  EXPECT_EQ(f[0], 0x3B);   // SOF B
  EXPECT_EQ(f[1], 0x16);   // address
  EXPECT_EQ(f[2], 0xEB);   // function WRITE
  EXPECT_EQ(f[3], 0x04);   // data length
  EXPECT_EQ(f[10], 0x0D);  // EOF 1
  EXPECT_EQ(f[11], 0x0A);  // EOF 2
}

// ── CRC ───────────────────────────────────────────────────────────────────────

TEST(WriteCommandTest, CrcBothOff) {
  TestableBasenBmsBle bms;
  // sum(0x16, 0xEB, 0x04, 0x86, 0x1D, 0x01, 0x00) = 425 = 0x01A9
  const uint8_t data[4] = {0x86, 0x1D, 0x01, 0x00};
  auto f = bms.build_frame_(0x3B, 0xEB, data, 4);
  EXPECT_EQ(f[8], 0xA9);
  EXPECT_EQ(f[9], 0x01);
}

TEST(WriteCommandTest, CrcChargingOn) {
  TestableBasenBmsBle bms;
  // sum(0x16, 0xEB, 0x04, 0x86, 0x1D, 0x01, 0x01) = 426 = 0x01AA
  const uint8_t data[4] = {0x86, 0x1D, 0x01, 0x01};
  auto f = bms.build_frame_(0x3B, 0xEB, data, 4);
  EXPECT_EQ(f[8], 0xAA);
  EXPECT_EQ(f[9], 0x01);
}

// ── Full frame comparisons against physical captures ─────────────────────────

TEST(WriteCommandTest, FrameBothOff) {
  TestableBasenBmsBle bms;
  const uint8_t data[4] = {0x86, 0x1D, 0x01, 0x00};
  auto f = bms.build_frame_(0x3B, 0xEB, data, 4);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3B, 0x16, 0xEB, 0x04, 0x86, 0x1D, 0x01, 0x00, 0xA9, 0x01, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(WriteCommandTest, FrameChargingOn) {
  TestableBasenBmsBle bms;
  const uint8_t data[4] = {0x86, 0x1D, 0x01, 0x01};
  auto f = bms.build_frame_(0x3B, 0xEB, data, 4);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3B, 0x16, 0xEB, 0x04, 0x86, 0x1D, 0x01, 0x01, 0xAA, 0x01, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(WriteCommandTest, FrameDischargingOn) {
  TestableBasenBmsBle bms;
  const uint8_t data[4] = {0x86, 0x1D, 0x01, 0x02};
  auto f = bms.build_frame_(0x3B, 0xEB, data, 4);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3B, 0x16, 0xEB, 0x04, 0x86, 0x1D, 0x01, 0x02, 0xAB, 0x01, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

TEST(WriteCommandTest, FrameBothOn) {
  TestableBasenBmsBle bms;
  const uint8_t data[4] = {0x86, 0x1D, 0x01, 0x03};
  auto f = bms.build_frame_(0x3B, 0xEB, data, 4);

  // clang-format off
  const std::vector<uint8_t> expected = {
      0x3B, 0x16, 0xEB, 0x04, 0x86, 0x1D, 0x01, 0x03, 0xAC, 0x01, 0x0D, 0x0A,
  };
  // clang-format on
  EXPECT_EQ(f, expected);
}

// ── change_mosfet_status state transitions ───────────────────────────────────

TEST(MosfetSwitchTest, ChargingOnFromBothOff) {
  TestableBasenBmsBle bms;
  bms.set_mosfet_status(0x00);
  bms.change_mosfet_status(0x011D, 0, true);  // bit 0 = charging
  EXPECT_EQ(bms.get_mosfet_status(), 0x01);
  EXPECT_EQ(bms.last_write_reg, 0x011D);
  EXPECT_EQ(bms.last_write_value, 0x01);
}

TEST(MosfetSwitchTest, DischargingOnFromBothOff) {
  TestableBasenBmsBle bms;
  bms.set_mosfet_status(0x00);
  bms.change_mosfet_status(0x011D, 1, true);  // bit 1 = discharging
  EXPECT_EQ(bms.get_mosfet_status(), 0x02);
  EXPECT_EQ(bms.last_write_reg, 0x011D);
  EXPECT_EQ(bms.last_write_value, 0x02);
}

TEST(MosfetSwitchTest, BothOnFromBothOff) {
  TestableBasenBmsBle bms;
  bms.set_mosfet_status(0x00);
  bms.change_mosfet_status(0x011D, 0, true);
  bms.change_mosfet_status(0x011D, 1, true);
  EXPECT_EQ(bms.get_mosfet_status(), 0x03);
  EXPECT_EQ(bms.last_write_value, 0x03);
}

TEST(MosfetSwitchTest, ChargingOffFromBothOn) {
  TestableBasenBmsBle bms;
  bms.set_mosfet_status(0x03);
  bms.change_mosfet_status(0x011D, 0, false);
  EXPECT_EQ(bms.get_mosfet_status(), 0x02);
  EXPECT_EQ(bms.last_write_value, 0x02);
}

TEST(MosfetSwitchTest, DischargingOffFromBothOn) {
  TestableBasenBmsBle bms;
  bms.set_mosfet_status(0x03);
  bms.change_mosfet_status(0x011D, 1, false);
  EXPECT_EQ(bms.get_mosfet_status(), 0x01);
  EXPECT_EQ(bms.last_write_value, 0x01);
}

TEST(MosfetSwitchTest, BothOffFromBothOn) {
  TestableBasenBmsBle bms;
  bms.set_mosfet_status(0x03);
  bms.change_mosfet_status(0x011D, 0, false);
  bms.change_mosfet_status(0x011D, 1, false);
  EXPECT_EQ(bms.get_mosfet_status(), 0x00);
  EXPECT_EQ(bms.last_write_value, 0x00);
}

TEST(MosfetSwitchTest, UnknownStatusReturnsFalse) {
  TestableBasenBmsBle bms;
  // mosfet_status_ defaults to 0xFF (unknown)
  bool result = bms.change_mosfet_status(0x011D, 0, true);
  EXPECT_FALSE(result);
}

}  // namespace esphome::basen_bms_ble::testing
