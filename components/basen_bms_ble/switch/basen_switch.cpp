#include "basen_switch.h"

namespace esphome::basen_bms_ble {

void BasenSwitch::write_state(bool state) {
  this->parent_->change_mosfet_status(this->holding_register_, this->bit_, state);
}

}  // namespace esphome::basen_bms_ble
