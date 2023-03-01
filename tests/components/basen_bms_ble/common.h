#pragma once
#include "esphome/components/basen_bms_ble/basen_bms_ble.h"
#include "esphome/components/switch/switch.h"

namespace esphome::basen_bms_ble::testing {

class TestableBasenBmsBle : public BasenBmsBle {
 public:
  void update() override {}
  bool send_command_(uint8_t start_of_frame, uint8_t function, uint8_t value = 0x00) { return false; }
  bool write_register(uint16_t reg, uint8_t value) {
    last_write_reg = reg;
    last_write_value = value;
    return true;
  }
  void set_mosfet_status(uint8_t status) { this->mosfet_status_ = status; }
  uint8_t get_mosfet_status() const { return this->mosfet_status_; }
  using BasenBmsBle::build_frame_;
  uint16_t last_write_reg{0};
  uint8_t last_write_value{0};
};

class TestableSwitch : public switch_::Switch {
 public:
  void write_state(bool state) override { this->publish_state(state); }
};

}  // namespace esphome::basen_bms_ble::testing
