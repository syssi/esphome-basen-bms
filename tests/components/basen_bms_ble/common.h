#pragma once
#include "esphome/components/basen_bms_ble/basen_bms_ble.h"
#include "esphome/components/switch/switch.h"

namespace esphome::basen_bms_ble::testing {

class TestableBasenBmsBle : public BasenBmsBle {
 public:
  void update() override {}
  bool send_command_(uint8_t start_of_frame, uint8_t function, uint8_t value = 0x00) { return false; }
};

class TestableSwitch : public switch_::Switch {
 public:
  void write_state(bool state) override { this->publish_state(state); }
};

}  // namespace esphome::basen_bms_ble::testing
