#include "basen_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::basen_bms_ble {

ESPHOME_LOG_TAG(TAG, "basen_bms_ble.switch");

void BasenSwitch::dump_config() { LOG_SWITCH("", "BasenBmsBle Switch", this); }
void BasenSwitch::write_state(bool state) {
  this->parent_->change_mosfet_status(this->holding_register_, this->bit_, state);
}

}  // namespace esphome::basen_bms_ble
