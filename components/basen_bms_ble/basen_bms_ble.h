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
  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging_binary_sensor) {
    charging_binary_sensor_ = charging_binary_sensor;
  }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging_binary_sensor) {
    discharging_binary_sensor_ = discharging_binary_sensor;
  }

  void set_total_voltage_sensor(sensor::Sensor *total_voltage_sensor) { total_voltage_sensor_ = total_voltage_sensor; }
  void set_current_sensor(sensor::Sensor *current_sensor) { current_sensor_ = current_sensor; }
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_charging_power_sensor(sensor::Sensor *charging_power_sensor) {
    charging_power_sensor_ = charging_power_sensor;
  }
  void set_discharging_power_sensor(sensor::Sensor *discharging_power_sensor) {
    discharging_power_sensor_ = discharging_power_sensor;
  }
  void set_capacity_remaining_sensor(sensor::Sensor *capacity_remaining_sensor) {
    capacity_remaining_sensor_ = capacity_remaining_sensor;
  }
  void set_charging_states_bitmask_sensor(sensor::Sensor *charging_states_bitmask_sensor) {
    charging_states_bitmask_sensor_ = charging_states_bitmask_sensor;
  }
  void set_discharging_states_bitmask_sensor(sensor::Sensor *discharging_states_bitmask_sensor) {
    discharging_states_bitmask_sensor_ = discharging_states_bitmask_sensor;
  }
  void set_charging_warnings_bitmask_sensor(sensor::Sensor *charging_warnings_bitmask_sensor) {
    charging_warnings_bitmask_sensor_ = charging_warnings_bitmask_sensor;
  }
  void set_discharging_warnings_bitmask_sensor(sensor::Sensor *discharging_warnings_bitmask_sensor) {
    discharging_warnings_bitmask_sensor_ = discharging_warnings_bitmask_sensor;
  }
  void set_state_of_charge_sensor(sensor::Sensor *state_of_charge_sensor) {
    state_of_charge_sensor_ = state_of_charge_sensor;
  }
  void set_nominal_capacity_sensor(sensor::Sensor *nominal_capacity_sensor) {
    nominal_capacity_sensor_ = nominal_capacity_sensor;
  }
  void set_nominal_voltage_sensor(sensor::Sensor *nominal_voltage_sensor) {
    nominal_voltage_sensor_ = nominal_voltage_sensor;
  }
  void set_real_capacity_sensor(sensor::Sensor *real_capacity_sensor) { real_capacity_sensor_ = real_capacity_sensor; }
  void set_serial_number_sensor(sensor::Sensor *serial_number_sensor) { serial_number_sensor_ = serial_number_sensor; }
  void set_charging_cycles_sensor(sensor::Sensor *charging_cycles_sensor) {
    charging_cycles_sensor_ = charging_cycles_sensor;
  }

  void set_min_cell_voltage_sensor(sensor::Sensor *min_cell_voltage_sensor) {
    min_cell_voltage_sensor_ = min_cell_voltage_sensor;
  }
  void set_max_cell_voltage_sensor(sensor::Sensor *max_cell_voltage_sensor) {
    max_cell_voltage_sensor_ = max_cell_voltage_sensor;
  }
  void set_min_voltage_cell_sensor(sensor::Sensor *min_voltage_cell_sensor) {
    min_voltage_cell_sensor_ = min_voltage_cell_sensor;
  }
  void set_max_voltage_cell_sensor(sensor::Sensor *max_voltage_cell_sensor) {
    max_voltage_cell_sensor_ = max_voltage_cell_sensor;
  }
  void set_delta_cell_voltage_sensor(sensor::Sensor *delta_cell_voltage_sensor) {
    delta_cell_voltage_sensor_ = delta_cell_voltage_sensor;
  }
  void set_average_cell_voltage_sensor(sensor::Sensor *average_cell_voltage_sensor) {
    average_cell_voltage_sensor_ = average_cell_voltage_sensor;
  }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage_sensor) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage_sensor;
  }
  void set_temperature_sensor(uint8_t temperature, sensor::Sensor *temperature_sensor) {
    this->temperatures_[temperature].temperature_sensor_ = temperature_sensor;
  }

  void set_charging_switch(switch_::Switch *charging_switch) { charging_switch_ = charging_switch; }
  void set_discharging_switch(switch_::Switch *discharging_switch) { discharging_switch_ = discharging_switch; }

  void set_charging_states_text_sensor(text_sensor::TextSensor *charging_states_text_sensor) {
    charging_states_text_sensor_ = charging_states_text_sensor;
  }
  void set_discharging_states_text_sensor(text_sensor::TextSensor *discharging_states_text_sensor) {
    discharging_states_text_sensor_ = discharging_states_text_sensor;
  }
  void set_charging_warnings_text_sensor(text_sensor::TextSensor *charging_warnings_text_sensor) {
    charging_warnings_text_sensor_ = charging_warnings_text_sensor;
  }
  void set_discharging_warnings_text_sensor(text_sensor::TextSensor *discharging_warnings_text_sensor) {
    discharging_warnings_text_sensor_ = discharging_warnings_text_sensor;
  }
  void set_manufacturing_date_text_sensor(text_sensor::TextSensor *manufacturing_date_text_sensor) {
    manufacturing_date_text_sensor_ = manufacturing_date_text_sensor;
  }

  void set_enable_fake_traffic(bool enable_fake_traffic) { enable_fake_traffic_ = enable_fake_traffic; }
  void write_register(uint8_t address, uint16_t value);

 protected:
  binary_sensor::BinarySensor *balancing_binary_sensor_;
  binary_sensor::BinarySensor *charging_binary_sensor_;
  binary_sensor::BinarySensor *discharging_binary_sensor_;

  sensor::Sensor *total_voltage_sensor_;
  sensor::Sensor *current_sensor_;
  sensor::Sensor *power_sensor_;
  sensor::Sensor *charging_power_sensor_;
  sensor::Sensor *discharging_power_sensor_;
  sensor::Sensor *capacity_remaining_sensor_;
  sensor::Sensor *charging_states_bitmask_sensor_;
  sensor::Sensor *discharging_states_bitmask_sensor_;
  sensor::Sensor *charging_warnings_bitmask_sensor_;
  sensor::Sensor *discharging_warnings_bitmask_sensor_;
  sensor::Sensor *state_of_charge_sensor_;
  sensor::Sensor *nominal_capacity_sensor_;
  sensor::Sensor *nominal_voltage_sensor_;
  sensor::Sensor *real_capacity_sensor_;
  sensor::Sensor *serial_number_sensor_;
  sensor::Sensor *charging_cycles_sensor_;
  sensor::Sensor *min_cell_voltage_sensor_;
  sensor::Sensor *max_cell_voltage_sensor_;
  sensor::Sensor *min_voltage_cell_sensor_;
  sensor::Sensor *max_voltage_cell_sensor_;
  sensor::Sensor *delta_cell_voltage_sensor_;
  sensor::Sensor *average_cell_voltage_sensor_;

  switch_::Switch *charging_switch_;
  switch_::Switch *discharging_switch_;

  text_sensor::TextSensor *charging_states_text_sensor_;
  text_sensor::TextSensor *discharging_states_text_sensor_;
  text_sensor::TextSensor *charging_warnings_text_sensor_;
  text_sensor::TextSensor *discharging_warnings_text_sensor_;
  text_sensor::TextSensor *manufacturing_date_text_sensor_;

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[34];

  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[4];

  std::vector<uint8_t> frame_buffer_;
  uint16_t char_notify_handle_;
  uint16_t char_command_handle_;
  uint8_t next_command_{5};
  bool enable_fake_traffic_;

  float min_cell_voltage_{100.0f};
  float max_cell_voltage_{-100.0f};
  uint8_t max_voltage_cell_{0};
  uint8_t min_voltage_cell_{0};

  void assemble_(const uint8_t *data, uint16_t length);
  void on_basen_bms_ble_data_(const std::vector<uint8_t> &data);
  void decode_status_data_(const std::vector<uint8_t> &data);
  void decode_general_info_data_(const std::vector<uint8_t> &data);
  void decode_cell_voltages_data_(const std::vector<uint8_t> &data);
  void decode_balancing_data_(const std::vector<uint8_t> &data);
  void decode_protect_ic_data_(const std::vector<uint8_t> &data);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_state_(switch_::Switch *obj, const bool &state);
  void inject_fake_traffic_(uint8_t frame_type);
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
