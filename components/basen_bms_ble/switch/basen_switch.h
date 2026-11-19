#pragma once

#include "../basen_bms_ble.h"
#include "esphome/components/switch/switch.h"

namespace esphome::basen_bms_ble {

class BasenBmsBle;
class BasenSwitch : public switch_::Switch {
 public:
  void set_parent(BasenBmsBle *parent) { this->parent_ = parent; };
  void set_holding_register(uint16_t holding_register) { this->holding_register_ = holding_register; };
  void set_bit(uint8_t bit) { this->bit_ = bit; };

 protected:
  void write_state(bool state) override;
  BasenBmsBle *parent_;
  uint16_t holding_register_;
  uint8_t bit_;
};

}  // namespace esphome::basen_bms_ble
