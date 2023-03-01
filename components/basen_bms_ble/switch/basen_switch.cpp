#include "basen_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome::basen_bms_ble {

static const char *const TAG = "basen_bms_ble.switch";

void BasenSwitch::dump_config() { LOG_SWITCH("", "BasenBmsBle Switch", this); }
void BasenSwitch::write_state(bool state) {
  this->parent_->change_mosfet_status(this->holding_register_, this->bit_, state);
}

}  // namespace esphome::basen_bms_ble
