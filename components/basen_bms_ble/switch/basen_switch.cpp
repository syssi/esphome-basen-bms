#include "basen_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace basen_bms_ble {

static const char *const TAG = "basen_bms_ble.switch";

void BasenSwitch::dump_config() { LOG_SWITCH("", "BasenBmsBle Switch", this); }
void BasenSwitch::write_state(bool state) {
  // this->parent_->write_register(this->holding_register_, (uint16_t) state);
}

}  // namespace basen_bms_ble
}  // namespace esphome
