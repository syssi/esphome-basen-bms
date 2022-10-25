#pragma once

#include "esphome/core/component.h"
#include "esphome/components/ble_client/ble_client.h"
#include "esphome/components/esp32_ble_tracker/esp32_ble_tracker.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#ifdef USE_ESP32

#include <esp_gattc_api.h>

namespace esphome {
namespace basen_bms_ble {

namespace espbt = esphome::esp32_ble_tracker;

class BasenBmsBle : public esphome::ble_client::BLEClientNode, public PollingComponent {
 public:
  void gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                           esp_ble_gattc_cb_param_t *param) override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing_binary_sensor) {
    balancing_binary_sensor_ = balancing_binary_sensor;
  }

  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }

  void set_charging_switch(switch_::Switch *charging_switch) { charging_switch_ = charging_switch; }

  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }

  void set_enable_fake_traffic(bool enable_fake_traffic) { enable_fake_traffic_ = enable_fake_traffic; }
  void write_register(uint8_t address, uint16_t value);

 protected:
  binary_sensor::BinarySensor *balancing_binary_sensor_;

  sensor::Sensor *total_voltage_sensor_;

  switch_::Switch *charging_switch_;

  text_sensor::TextSensor *errors_text_sensor_;

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[32];

  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[6];

  std::vector<uint8_t> frame_buffer_;
  uint16_t char_notify_handle_;
  uint16_t char_command_handle_;
  uint8_t next_command_{0};
  bool enable_fake_traffic_;

  void assemble_(const uint8_t *data, uint16_t length);
  void on_basen_bms_ble_data_(const std::vector<uint8_t> &data);
  void decode_status_data_(const std::vector<uint8_t> &data);
  void decode_general_info_data_(const std::vector<uint8_t> &data);
  void decode_cell_voltages_data_(const std::vector<uint8_t> &data);
  void decode_protect_ic_data_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  bool send_command_(uint8_t start_of_frame, uint8_t function, uint8_t value = 0x00);
  std::string charging_states_bits_to_string_(uint8_t mask);
  std::string discharging_states_bits_to_string_(uint8_t mask);
  std::string charging_warnings_bits_to_string_(uint8_t mask);
  std::string discharging_warnings_bits_to_string_(uint8_t mask);

  uint16_t chksum_(const uint8_t data[], const uint16_t len) {
    uint16_t checksum = 0x00;
    for (uint16_t i = 0; i < len; i++) {
      checksum = checksum + data[i];
    }
    return checksum;
  }
};

}  // namespace basen_bms_ble
}  // namespace esphome

#endif
