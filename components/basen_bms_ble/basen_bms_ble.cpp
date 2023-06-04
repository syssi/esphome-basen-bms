#include "basen_bms_ble.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace basen_bms_ble {

static const char *const TAG = "basen_bms_ble";

static const uint16_t BASEN_BMS_SERVICE_UUID = 0xFA00;
static const uint16_t BASEN_BMS_NOTIFY_CHARACTERISTIC_UUID = 0xFA01;   // handle 0x12
static const uint16_t BASEN_BMS_CONTROL_CHARACTERISTIC_UUID = 0xFA02;  // handle 0x15

static const uint16_t MAX_RESPONSE_SIZE = 42 + 2;

static const uint8_t BASEN_PKT_START_A = 0x3A;
static const uint8_t BASEN_PKT_START_B = 0x3B;
static const uint8_t BASEN_ADDRESS = 0x16;
static const uint8_t BASEN_PKT_END_1 = 0x0D;
static const uint8_t BASEN_PKT_END_2 = 0x0A;

static const uint8_t BASEN_FRAME_TYPE_CELL_VOLTAGES_1_12 = 0x24;
static const uint8_t BASEN_FRAME_TYPE_CELL_VOLTAGES_13_24 = 0x25;
static const uint8_t BASEN_FRAME_TYPE_CELL_VOLTAGES_25_34 = 0x26;
static const uint8_t BASEN_FRAME_TYPE_PROTECT_IC = 0x27;
static const uint8_t BASEN_FRAME_TYPE_STATUS = 0x2A;
static const uint8_t BASEN_FRAME_TYPE_GENERAL_INFO = 0x2B;
static const uint8_t BASEN_FRAME_TYPE_SETTINGS = 0xE8;
static const uint8_t BASEN_FRAME_TYPE_SETTINGS_ALTERNATIVE = 0xEA;
static const uint8_t BASEN_FRAME_TYPE_BALANCING = 0xFE;

static const uint8_t BASEN_COMMAND_QUEUE_SIZE = 5;
static const uint8_t BASEN_COMMAND_QUEUE[BASEN_COMMAND_QUEUE_SIZE] = {
    BASEN_FRAME_TYPE_STATUS,
    BASEN_FRAME_TYPE_GENERAL_INFO,
    BASEN_FRAME_TYPE_CELL_VOLTAGES_1_12,
    BASEN_FRAME_TYPE_CELL_VOLTAGES_13_24,
    BASEN_FRAME_TYPE_BALANCING,
};

static const uint8_t CHARGING_STATES_SIZE = 8;
static const char *const CHARGING_STATES[CHARGING_STATES_SIZE] = {
    "Overcurrent protection (SOCC)",  // 0000 0001
    "Over temperature (OTC)",         // 0000 0010
    "Undertemperature (UTC)",         // 0000 0100
    "Cell overvoltage (COV)",         // 0000 1000
    "Battery overvoltage (FC)",       // 0001 0000
    "Reserved",                       // 0010 0000
    "Reserved",                       // 0100 0000
    "Charging MOS (CHG)",             // 1000 0000
};

static const uint8_t CHARGING_WARNINGS_SIZE = 8;
static const char *const CHARGING_WARNINGS[CHARGING_WARNINGS_SIZE] = {
    "Overcurrent (OCC1)",          // 0000 0001
    "Over temperature (OTC)",      // 0000 0010
    "Undertemperature (UTC1)",     // 0000 0100
    "Differential Pressure (DP)",  // 0000 1000
    "Fully charged (FC)",          // 0001 0000
    "Reserved",                    // 0010 0000
    "Reserved",                    // 0100 0000
    "Reserved",                    // 1000 0000
};

static const uint8_t DISCHARGING_STATES_SIZE = 8;
static const char *const DISCHARGING_STATES[DISCHARGING_STATES_SIZE] = {
    "Overcurrent protection (SOCD)",    // 0000 0001
    "Over temperature (OTD)",           // 0000 0010
    "Undertemperature (UTD)",           // 0000 0100
    "Battery undervoltage (CUV)",       // 0000 1000
    "Battery empty (FD)",               // 0001 0000
    "Short circuit protection (ASCD)",  // 0010 0000
    "Termination of discharge (TDA)",   // 0100 0000
    "Discharging MOS (DSG)",            // 1000 0000
};

static const uint8_t DISCHARGING_WARNINGS_SIZE = 8;
static const char *const DISCHARGING_WARNINGS[DISCHARGING_WARNINGS_SIZE] = {
    "Overcurrent (OCD1)",                     // 0000 0001
    "Over temperature (OTD1)",                // 0000 0010
    "Undertemperature (UTD1)",                // 0000 0100
    "Differential Pressure (DP)",             // 0000 1000
    "Not enough time left (RTA)",             // 0001 0000
    "Insufficient capacity remaining (RCA)",  // 0010 0000
    "Battery undervoltage (CUV)",             // 0100 0000
    "Battery empty (FD)",                     // 1000 0000
};

void BasenBmsBle::gattc_event_handler(esp_gattc_cb_event_t event, esp_gatt_if_t gattc_if,
                                      esp_ble_gattc_cb_param_t *param) {
  switch (event) {
    case ESP_GATTC_OPEN_EVT: {
      break;
    }
    case ESP_GATTC_DISCONNECT_EVT: {
      this->node_state = espbt::ClientState::IDLE;

      // this->publish_state_(this->voltage_sensor_, NAN);
      break;
    }
    case ESP_GATTC_SEARCH_CMPL_EVT: {
      // [esp32_ble_client:048]: [0] [A4:C1:38:27:48:9A] Found device
      // [esp32_ble_client:064]: [0] [A4:C1:38:27:48:9A] 0x00 Attempting BLE connection
      // [esp32_ble_client:126]: [0] [A4:C1:38:27:48:9A] ESP_GATTC_OPEN_EVT
      // [esp32_ble_client:186]: [0] [A4:C1:38:27:48:9A] ESP_GATTC_SEARCH_CMPL_EVT
      // [esp32_ble_client:189]: [0] [A4:C1:38:27:48:9A] Service UUID: 0x1800
      // [esp32_ble_client:191]: [0] [A4:C1:38:27:48:9A] start_handle: 0x1 end_handle: 0x7
      // [esp32_ble_client:189]: [0] [A4:C1:38:27:48:9A] Service UUID: 0x1801
      // [esp32_ble_client:191]: [0] [A4:C1:38:27:48:9A] start_handle: 0x8 end_handle: 0xb
      // [esp32_ble_client:189]: [0] [A4:C1:38:27:48:9A] Service UUID: 0x180A
      // [esp32_ble_client:191]: [0] [A4:C1:38:27:48:9A] start_handle: 0xc end_handle: 0xe
      // [esp32_ble_client:189]: [0] [A4:C1:38:27:48:9A] Service UUID: 0xFA00
      // [esp32_ble_client:191]: [0] [A4:C1:38:27:48:9A] start_handle: 0xf end_handle: 0x16
      // [esp32_ble_client:193]: [0] [A4:C1:38:27:48:9A] Connected
      // [esp32_ble_client:069]: [0] [A4:C1:38:27:48:9A] characteristic 0xFA01, handle 0x11, properties 0x12
      // [esp32_ble_client:069]: [0] [A4:C1:38:27:48:9A] characteristic 0xFA02, handle 0x15, properties 0x6

      auto *char_notify =
          this->parent_->get_characteristic(BASEN_BMS_SERVICE_UUID, BASEN_BMS_NOTIFY_CHARACTERISTIC_UUID);
      if (char_notify == nullptr) {
        ESP_LOGE(TAG, "[%s] No notify service found at device, not an BASEN BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_notify_handle_ = char_notify->handle;

      auto status = esp_ble_gattc_register_for_notify(this->parent()->get_gattc_if(), this->parent()->get_remote_bda(),
                                                      char_notify->handle);
      if (status) {
        ESP_LOGW(TAG, "esp_ble_gattc_register_for_notify failed, status=%d", status);
      }

      auto *char_command =
          this->parent_->get_characteristic(BASEN_BMS_SERVICE_UUID, BASEN_BMS_CONTROL_CHARACTERISTIC_UUID);
      if (char_command == nullptr) {
        ESP_LOGE(TAG, "[%s] No control service found at device, not an BASEN BMS..?",
                 this->parent_->address_str().c_str());
        break;
      }
      this->char_command_handle_ = char_command->handle;
      break;
    }
    case ESP_GATTC_REG_FOR_NOTIFY_EVT: {
      this->node_state = espbt::ClientState::ESTABLISHED;

      // Write 3b162a010041000d0a to handle 0x15
      // Response 1: 3b162a1843040000cd68000015161919691f0000 + 8080000007020000c2030d0a
      this->send_command_(BASEN_PKT_START_B, BASEN_FRAME_TYPE_STATUS);

      // Write 3b162a010041000d0a to handle 0x15
      // Response 2: 3b162a1843040000cd68000015161919691f0000 + 8080000007020000c2030d0a
      // this->send_command_(BASEN_PKT_START_B, 0x2A);

      // Write 3b162a010041000d0a to handle 0x15
      // Response 3: 3b162a1843040000cd683a162418170d1b0d170d + 190d180d1b0d1c0d1c0d00000000000000008701 + 0d0a
      // this->send_command_(BASEN_PKT_START_B, 0x2A);

      break;
    }
    case ESP_GATTC_NOTIFY_EVT: {
      ESP_LOGVV(TAG, "Notification received (handle 0x%02X): %s", param->notify.handle,
                format_hex_pretty(param->notify.value, param->notify.value_len).c_str());

      this->assemble_(param->notify.value, param->notify.value_len);
      break;
    }
    default:
      break;
  }
}

void BasenBmsBle::assemble_(const uint8_t *data, uint16_t length) {
  if (this->frame_buffer_.size() > MAX_RESPONSE_SIZE) {
    ESP_LOGW(TAG, "Maximum response size exceeded");
    this->frame_buffer_.clear();
  }

  // Flush buffer on every preamble
  if (data[0] == BASEN_PKT_START_A || data[0] == BASEN_PKT_START_B) {
    this->frame_buffer_.clear();
  }

  this->frame_buffer_.insert(this->frame_buffer_.end(), data, data + length);

  if (this->frame_buffer_.back() == BASEN_PKT_END_2) {
    const uint8_t *raw = &this->frame_buffer_[0];

    uint16_t data_len = raw[3];
    uint16_t frame_len = 4 + data_len + 4;
    if (frame_len != this->frame_buffer_.size()) {
      ESP_LOGW(TAG, "Invalid frame length");
      this->frame_buffer_.clear();
      return;
    }

    uint16_t computed_crc = chksum_(raw + 1, data_len + 3);
    uint16_t remote_crc = uint16_t(raw[frame_len - 3]) << 8 | (uint16_t(raw[frame_len - 4]) << 0);
    if (computed_crc != remote_crc) {
      ESP_LOGW(TAG, "CRC check failed! 0x%04X != 0x%04X", computed_crc, remote_crc);
      this->frame_buffer_.clear();
      return;
    }

    std::vector<uint8_t> data(this->frame_buffer_.begin(), this->frame_buffer_.end() - 4);

    this->on_basen_bms_ble_data_(data);
    this->frame_buffer_.clear();
  }
}

void BasenBmsBle::update() {
  if (this->node_state != espbt::ClientState::ESTABLISHED && !this->enable_fake_traffic_) {
    ESP_LOGW(TAG, "[%s] Not connected", this->parent_->address_str().c_str());
    return;
  }

  // Loop through all commands if connected
  if (this->next_command_ != BASEN_COMMAND_QUEUE_SIZE) {
    ESP_LOGW(TAG,
             "Command queue (%d of %d) was not completely processed. "
             "Please increase the update_interval if you see this warning frequently",
             this->next_command_ + 1, BASEN_COMMAND_QUEUE_SIZE);
  }
  this->next_command_ = 0;
  this->send_command_(BASEN_PKT_START_A, BASEN_COMMAND_QUEUE[this->next_command_++ % BASEN_COMMAND_QUEUE_SIZE]);
}

void BasenBmsBle::on_basen_bms_ble_data_(const std::vector<uint8_t> &data) {
  uint8_t frame_type = data[2];

  switch (frame_type) {
    case BASEN_FRAME_TYPE_STATUS:
      this->decode_status_data_(data);
      break;
    case BASEN_FRAME_TYPE_GENERAL_INFO:
      this->decode_general_info_data_(data);
      break;
    case BASEN_FRAME_TYPE_CELL_VOLTAGES_1_12:
    case BASEN_FRAME_TYPE_CELL_VOLTAGES_13_24:
    case BASEN_FRAME_TYPE_CELL_VOLTAGES_25_34:
      this->decode_cell_voltages_data_(data);
      break;
    case BASEN_FRAME_TYPE_PROTECT_IC:
      this->decode_protect_ic_data_(data);
      break;
    case BASEN_FRAME_TYPE_BALANCING:
      this->decode_balancing_data_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unhandled response received (frame_type 0x%02X): %s", frame_type,
               format_hex_pretty(&data.front(), data.size()).c_str());
  }

  // Send next command after each received frame
  if (this->next_command_ < BASEN_COMMAND_QUEUE_SIZE) {
    this->send_command_(BASEN_PKT_START_A, BASEN_COMMAND_QUEUE[this->next_command_++ % BASEN_COMMAND_QUEUE_SIZE]);
  }
}

void BasenBmsBle::decode_status_data_(const std::vector<uint8_t> &data) {
  auto basen_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };
  auto basen_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(basen_get_16bit(i + 2)) << 16) | (uint32_t(basen_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "Status frame (%d+4 bytes):", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x3B                 Start of frame
  //  1    1  0x16                 Address
  //  2    1  0x2A                 Frame type
  //  3    1  0x18                 Data length
  //  4    4  0x00 0x00 0x00 0x00  Current (without calibration)    A     0.001f
  float current = ((int32_t) basen_get_32bit(4)) * 0.001f;
  this->publish_state_(this->current_sensor_, current);

  //  8    4  0xCE 0x61 0x00 0x00  Total voltage                    V     0.001f
  float total_voltage = basen_get_32bit(8) * 0.001f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, std::max(0.0f, power));               // 500W vs 0W -> 500W
  this->publish_state_(this->discharging_power_sensor_, std::abs(std::min(0.0f, power)));  // -500W vs 0W -> 500W

  //  12   1  0x12                 Temperature 1                    °C    1.0f
  //  13   1  0x14                 Temperature 2                    °C    1.0f
  //  14   1  0x19                 Temperature 3                    °C    1.0f
  //  15   1  0x19                 Temperature 4                    °C    1.0f
  for (uint8_t i = 0; i < 4; i++) {
    this->publish_state_(this->temperatures_[i].temperature_sensor_, (float) data[12 + i]);
  }

  //  16   4  0x63 0x23 0x00 0x00  Capacity remaining               Ah    0.001f
  this->publish_state_(this->capacity_remaining_sensor_, basen_get_32bit(16) * 0.001f);

  //  20   1  0x80                 Charging states (Bitmask)
  this->publish_state_(this->charging_states_bitmask_sensor_, data[20]);
  this->publish_state_(this->charging_states_text_sensor_, this->charging_states_bits_to_string_(data[20]));
  this->publish_state_(this->charging_binary_sensor_, (bool) (data[20] & (1 << 7)));
  this->publish_state_(this->charging_switch_, (bool) (data[20] & (1 << 7)));

  //  21   1  0x80                 Discharging states (Bitmask)
  this->publish_state_(this->discharging_states_bitmask_sensor_, data[21]);
  this->publish_state_(this->discharging_states_text_sensor_, this->discharging_states_bits_to_string_(data[21]));
  this->publish_state_(this->discharging_binary_sensor_, (bool) (data[21] & (1 << 7)));
  this->publish_state_(this->discharging_switch_, (bool) (data[21] & (1 << 7)));

  //  22   1  0x00                 Charging warnings (Bitmask)
  this->publish_state_(this->charging_warnings_bitmask_sensor_, data[22]);
  this->publish_state_(this->charging_warnings_text_sensor_, this->charging_states_bits_to_string_(data[22]));

  //  23   1  0x00                 Discharging warnings (Bitmask)
  this->publish_state_(this->discharging_warnings_bitmask_sensor_, data[23]);
  this->publish_state_(this->discharging_warnings_text_sensor_, this->charging_states_bits_to_string_(data[23]));

  //  24   1  0x08                 State of charge                  %     1.0f
  this->publish_state_(this->state_of_charge_sensor_, (float) data[24]);

  //  25   1  0x19                 Unused
  //  26   1  0x00                 Unused
  //  27   1  0x00                 Unused
  //  28   1  0x6F                 CRC
  //  29   1  0x03                 CRC
  //  30   1  0x0D                 End of frame
  //  31   1  0x0A                 End of frame
}

void BasenBmsBle::decode_general_info_data_(const std::vector<uint8_t> &data) {
  auto basen_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };
  auto basen_get_32bit = [&](size_t i) -> uint32_t {
    return (uint32_t(basen_get_16bit(i + 2)) << 16) | (uint32_t(basen_get_16bit(i + 0)) << 0);
  };

  ESP_LOGI(TAG, "General info frame (%d+4 bytes):", data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x3A                 Start of frame
  //  1    1  0x16                 Address
  //  2    1  0x2B                 Frame type
  //  3    1  0x18                 Data length
  //  4    4  0xA0 0x86 0x01 0x00  Nominal capacity                 Ah    0.001f
  this->publish_state_(this->nominal_capacity_sensor_, basen_get_32bit(4) * 0.001f);

  //  8    4  0x00 0x64 0x00 0x00  Nominal voltage                  V     0.001f
  this->publish_state_(this->nominal_voltage_sensor_, basen_get_32bit(8) * 0.001f);

  //  12   4  0x91 0xA0 0x01 0x00  Real capacity                    Ah    0.001f
  this->publish_state_(this->real_capacity_sensor_, basen_get_32bit(12) * 0.001f);

  //  16   1  0x00                 Unused
  //  17   1  0x00                 Unused
  //  18   1  0x00                 Unused
  //  19   1  0x00                 Unused
  //  20   1  0x30                 Unused
  //  21   1  0x75                 Unused
  //  22   2  0x00 0x00            Serial number
  this->publish_state_(this->serial_number_sensor_, (float) basen_get_16bit(22));

  //  24   2  0x71 0x53            Manufacturing date
  if (this->manufacturing_date_text_sensor_ != nullptr) {
    uint16_t raw_date = basen_get_16bit(24);
    uint16_t year = ((raw_date >> 9) & 127) + 1980;
    uint8_t month = (raw_date >> 5) & 15;
    uint8_t day = 31 & raw_date;
    this->publish_state_(this->manufacturing_date_text_sensor_,
                         to_string(year) + "." + to_string(month) + "." + to_string(day));
  }

  //  26   2  0x07 0x00            Charging cycles
  this->publish_state_(this->charging_cycles_sensor_, (float) basen_get_16bit(26));

  //  28   1  0x86                 CRC
  //  29   1  0x04                 CRC
  //  30   1  0x0D                 End of frame
  //  31   1  0x0A                 End of frame
}

void BasenBmsBle::decode_cell_voltages_data_(const std::vector<uint8_t> &data) {
  auto basen_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 1]) << 8) | (uint16_t(data[i + 0]) << 0);
  };

  uint8_t offset = 12 * (data[2] - 36);
  uint8_t cells = data[3] / 2;

  ESP_LOGI(TAG, "Cell voltages frame (chunk %d, %d+4 bytes):", data[2] - 36, data.size());
  ESP_LOGD(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x3A                 Start of frame
  //  1    1  0x16                 Address
  //  2    1  0x24                 Frame type
  //  3    1  0x18                 Data length
  //  4    2  0x96 0x0C            Cell voltage 1
  //  6    2  0x97 0x0C            Cell voltage 2
  //  8    2  0x98 0x0C            Cell voltage 3
  //  10   2  0x96 0x0C            Cell voltage 4
  //  12   2  0x96 0x0C            Cell voltage 5
  //  14   2  0x98 0x0C            Cell voltage 6
  //  16   2  0x98 0x0C            Cell voltage 7
  //  18   2  0x97 0x0C            Cell voltage 8
  //  20   2  0x00 0x00            Cell voltage 9
  //  22   1  0x00 0x00            Cell voltage 10
  //  24   1  0x00 0x00            Cell voltage 11
  //  26   1  0x00 0x00            Cell voltage 12
  for (uint8_t i = 0; i < cells; i++) {
    float cell_voltage = basen_get_16bit((i * 2) + 4) * 0.001f;
    if (cell_voltage > 0 && cell_voltage < this->min_cell_voltage_) {
      this->min_cell_voltage_ = cell_voltage;
      this->min_voltage_cell_ = i + offset + 1;
    }
    if (cell_voltage > this->max_cell_voltage_) {
      this->max_cell_voltage_ = cell_voltage;
      this->max_voltage_cell_ = i + offset + 1;
    }
    this->publish_state_(this->cells_[i + offset].cell_voltage_sensor_, cell_voltage);
  }

  // Publish aggregated sensors at the last chunk. Must be improved if 3 chunks are retrieved.
  if (data[2] == 0x25) {
    this->publish_state_(this->min_cell_voltage_sensor_, this->min_cell_voltage_);
    this->publish_state_(this->max_cell_voltage_sensor_, this->max_cell_voltage_);
    this->publish_state_(this->max_voltage_cell_sensor_, (float) this->max_voltage_cell_);
    this->publish_state_(this->min_voltage_cell_sensor_, (float) this->min_voltage_cell_);
    this->publish_state_(this->delta_cell_voltage_sensor_, this->max_cell_voltage_ - this->min_cell_voltage_);
  }

  //  28   1  0x6A                 CRC
  //  29   1  0x05                 CRC
  //  30   1  0x0D                 End of frame
  //  31   1  0x0A                 End of frame
}

void BasenBmsBle::decode_balancing_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Balancing frame (%d+4 bytes):", data.size());
  ESP_LOGI(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x3A                 Start of frame
  //  1    1  0x16                 Address
  //  2    1  0xFE                 Frame type
  //  3    1  0x13                 Data length
  //  4    1  0x01
  //  5    1  0x75
  //  6    1  0x08
  //  7    1  0x34
  //  8    1  0x80
  //  9    1  0x80
  //  10   1  0x00
  //  11   1  0x00
  //  12   1  0x80
  //  13   1  0x00
  //  14   1  0x00
  //  15   1  0x00
  //  16   1  0x00
  //  17   1  0x00
  //  18   1  0x00
  //  19   1  0x02
  //  20   1  0x76
  //  21   1  0x53
  //  22   1  0x61
  //  23   1  0x85                 CRC
  //  24   1  0x04                 CRC
  //  25   1  0x0D                 End of frame
  //  26   1  0x0A                 End of frame
}

void BasenBmsBle::decode_protect_ic_data_(const std::vector<uint8_t> &data) {
  ESP_LOGI(TAG, "Protect IC frame (%d+4 bytes):", data.size());
  ESP_LOGI(TAG, "  %s", format_hex_pretty(&data.front(), data.size()).c_str());

  // Byte Len Payload              Description                      Unit  Precision
  //  0    1  0x3A                 Start of frame
  //  1    1  0x16                 Address
  //  2    1  0x27                 Frame type
  //  3    1                       Data length
  //  4    1
  //  5    1
  //  6    1
  //  7    1
  //  8    1
  //  9    1
  //  10   1
  //  11   1
  //  12   1
  //  13   1                       Temperature 3 (if SOF is 0x3B)
  //  14   1
  //  15   1
  //  16   1
  //  17   1                       Temperature 4 (if SOF is 0x3B)
  //  18   1
  //  19   1
  //  20   1
  //  21   1
  //  22   1
  //  23   1                       CRC
  //  24   1  0x04                 CRC
  //  25   1  0x0D                 End of frame
  //  26   1  0x0A                 End of frame
}

void BasenBmsBle::dump_config() {  // NOLINT(google-readability-function-size,readability-function-size)
  ESP_LOGCONFIG(TAG, "BasenBmsBle:");
  ESP_LOGCONFIG(TAG, "  Fake traffic enabled: %s", YESNO(this->enable_fake_traffic_));

  LOG_BINARY_SENSOR("", "Balancing", this->balancing_binary_sensor_);
  LOG_BINARY_SENSOR("", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("", "Discharging", this->discharging_binary_sensor_);

  LOG_SENSOR("", "Total voltage", this->total_voltage_sensor_);
  LOG_SENSOR("", "Current", this->current_sensor_);
  LOG_SENSOR("", "Power", this->power_sensor_);
  LOG_SENSOR("", "Charging power", this->charging_power_sensor_);
  LOG_SENSOR("", "Discharging power", this->discharging_power_sensor_);
  LOG_SENSOR("", "Capacity remaining", this->capacity_remaining_sensor_);
  LOG_SENSOR("", "Charging states bitmask", this->charging_states_bitmask_sensor_);
  LOG_SENSOR("", "Discharging states bitmask", this->discharging_states_bitmask_sensor_);
  LOG_SENSOR("", "Charging warnings bitmask", this->charging_warnings_bitmask_sensor_);
  LOG_SENSOR("", "Discharging warnings bitmask", this->discharging_warnings_bitmask_sensor_);
  LOG_SENSOR("", "State of charge", this->state_of_charge_sensor_);
  LOG_SENSOR("", "Nominal capacity", this->nominal_capacity_sensor_);
  LOG_SENSOR("", "Nominal voltage", this->nominal_voltage_sensor_);
  LOG_SENSOR("", "Real capacity", this->real_capacity_sensor_);
  LOG_SENSOR("", "Serial number", this->serial_number_sensor_);
  LOG_SENSOR("", "Charging cycles", this->charging_cycles_sensor_);
  LOG_SENSOR("", "Min cell voltage", this->min_cell_voltage_sensor_);
  LOG_SENSOR("", "Max cell voltage", this->max_cell_voltage_sensor_);
  LOG_SENSOR("", "Min voltage cell", this->min_voltage_cell_sensor_);
  LOG_SENSOR("", "Max voltage cell", this->max_voltage_cell_sensor_);
  LOG_SENSOR("", "Delta cell voltage", this->delta_cell_voltage_sensor_);
  LOG_SENSOR("", "Temperature 1", this->temperatures_[0].temperature_sensor_);
  LOG_SENSOR("", "Temperature 2", this->temperatures_[1].temperature_sensor_);
  LOG_SENSOR("", "Temperature 3", this->temperatures_[2].temperature_sensor_);
  LOG_SENSOR("", "Temperature 4", this->temperatures_[3].temperature_sensor_);
  LOG_SENSOR("", "Cell Voltage 1", this->cells_[0].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 2", this->cells_[1].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 3", this->cells_[2].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 4", this->cells_[3].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 5", this->cells_[4].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 6", this->cells_[5].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 7", this->cells_[6].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 8", this->cells_[7].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 9", this->cells_[8].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 10", this->cells_[9].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 11", this->cells_[10].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 12", this->cells_[11].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 13", this->cells_[12].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 14", this->cells_[13].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 15", this->cells_[14].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 16", this->cells_[15].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 17", this->cells_[16].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 18", this->cells_[17].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 19", this->cells_[18].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 20", this->cells_[19].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 21", this->cells_[20].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 22", this->cells_[21].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 23", this->cells_[22].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 24", this->cells_[23].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 25", this->cells_[24].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 26", this->cells_[25].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 27", this->cells_[26].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 28", this->cells_[27].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 29", this->cells_[28].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 30", this->cells_[29].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 31", this->cells_[30].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 32", this->cells_[31].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 33", this->cells_[32].cell_voltage_sensor_);
  LOG_SENSOR("", "Cell Voltage 34", this->cells_[33].cell_voltage_sensor_);

  LOG_TEXT_SENSOR("", "Charging states", this->charging_states_text_sensor_);
  LOG_TEXT_SENSOR("", "Discharging states", this->discharging_states_text_sensor_);
  LOG_TEXT_SENSOR("", "Charging warnings", this->charging_warnings_text_sensor_);
  LOG_TEXT_SENSOR("", "Discharging warnings", this->discharging_warnings_text_sensor_);
}

void BasenBmsBle::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void BasenBmsBle::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void BasenBmsBle::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

void BasenBmsBle::publish_state_(switch_::Switch *obj, const bool &state) {
  if (obj == nullptr)
    return;

  obj->publish_state(state);
}

void BasenBmsBle::write_register(uint8_t address, uint16_t value) {
  // this->send_command_(BASEN_CMD_WRITE, BASEN_CMD_MOS);  // @TODO: Pass value
}

bool BasenBmsBle::send_command_(uint8_t start_of_frame, uint8_t function, uint8_t value) {
  uint8_t frame[9];
  uint8_t data_len = 1;

  frame[0] = start_of_frame;
  frame[1] = BASEN_ADDRESS;
  frame[2] = function;
  frame[3] = data_len;
  frame[4] = value;
  auto crc = chksum_(frame + 1, 4);
  frame[5] = crc >> 0;
  frame[6] = crc >> 8;
  frame[7] = BASEN_PKT_END_1;
  frame[8] = BASEN_PKT_END_2;

  ESP_LOGV(TAG, "Send command (handle 0x%02X): %s", this->char_command_handle_,
           format_hex_pretty(frame, sizeof(frame)).c_str());

  if (this->enable_fake_traffic_) {
    this->inject_fake_traffic_(function);
    return true;
  }

  auto status =
      esp_ble_gattc_write_char(this->parent_->get_gattc_if(), this->parent_->get_conn_id(), this->char_command_handle_,
                               sizeof(frame), frame, ESP_GATT_WRITE_TYPE_NO_RSP, ESP_GATT_AUTH_REQ_NONE);

  if (status) {
    ESP_LOGW(TAG, "[%s] esp_ble_gattc_write_char failed, status=%d", this->parent_->address_str().c_str(), status);
  }

  return (status == 0);
}

void BasenBmsBle::inject_fake_traffic_(uint8_t frame_type) {
  // Current -6909 mAh
  const uint8_t status_frame[32] = {0x3a, 0x16, 0x2a, 0x18, 0x03, 0xe5, 0xff, 0xff, 0x06, 0x64, 0x00,
                                    0x00, 0x12, 0x14, 0x19, 0x19, 0x35, 0x3d, 0x00, 0x00, 0x80, 0x80,
                                    0x00, 0x00, 0x0e, 0x02, 0x00, 0x00, 0x82, 0x05, 0x0d, 0x0a};
  const uint8_t general_info_frame[32] = {0x3a, 0x16, 0x2b, 0x18, 0xa0, 0x86, 0x01, 0x00, 0x00, 0x64, 0x00,
                                          0x00, 0x91, 0xa0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x75,
                                          0x00, 0x00, 0x71, 0x53, 0x07, 0x00, 0x86, 0x04, 0x0d, 0x0a};
  const uint8_t cell_voltages_frame[32] = {0x3a, 0x16, 0x24, 0x18, 0x96, 0x0c, 0x97, 0x0c, 0x98, 0x0c, 0x96,
                                           0x0c, 0x96, 0x0c, 0x98, 0x0c, 0x98, 0x0c, 0x97, 0x0c, 0x00, 0x00,
                                           0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6a, 0x05, 0x0d, 0x0a};
  const uint8_t cell_voltages_frame2[32] = {0x3a, 0x16, 0x25, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x00, 0x0d, 0x0a};
  const uint8_t balancing_frame[27] = {0x3a, 0x16, 0xfe, 0x13, 0x00, 0xf9, 0x0f, 0x2c, 0x80,
                                       0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
                                       0x00, 0x02, 0x76, 0x53, 0x61, 0x07, 0x05, 0x0d, 0x0a};

  switch (frame_type) {
    case BASEN_FRAME_TYPE_STATUS:
      this->assemble_(status_frame, sizeof(status_frame));
      break;
    case BASEN_FRAME_TYPE_GENERAL_INFO:
      this->assemble_(general_info_frame, sizeof(general_info_frame));
      break;
    case BASEN_FRAME_TYPE_CELL_VOLTAGES_1_12:
      this->assemble_(cell_voltages_frame, sizeof(cell_voltages_frame));
      break;
    case BASEN_FRAME_TYPE_CELL_VOLTAGES_13_24:
      this->assemble_(cell_voltages_frame2, sizeof(cell_voltages_frame2));
      break;
    case BASEN_FRAME_TYPE_BALANCING:
      this->assemble_(balancing_frame, sizeof(balancing_frame));
      break;
    default:
      ESP_LOGW(TAG, "Unhandled request received: 0x%02X", frame_type);
  }
}

std::string BasenBmsBle::charging_states_bits_to_string_(const uint8_t mask) {
  std::string values = "";
  if (mask) {
    for (int i = 0; i < CHARGING_STATES_SIZE; i++) {
      if (mask & (1 << i)) {
        values.append(CHARGING_STATES[i]);
        values.append(";");
      }
    }
    if (!values.empty()) {
      values.pop_back();
    }
  }
  return values;
}

std::string BasenBmsBle::discharging_states_bits_to_string_(const uint8_t mask) {
  std::string values = "";
  if (mask) {
    for (int i = 0; i < DISCHARGING_STATES_SIZE; i++) {
      if (mask & (1 << i)) {
        values.append(DISCHARGING_STATES[i]);
        values.append(";");
      }
    }
    if (!values.empty()) {
      values.pop_back();
    }
  }
  return values;
}

std::string BasenBmsBle::charging_warnings_bits_to_string_(const uint8_t mask) {
  std::string values = "";
  if (mask) {
    for (int i = 0; i < CHARGING_WARNINGS_SIZE; i++) {
      if (mask & (1 << i)) {
        values.append(CHARGING_WARNINGS[i]);
        values.append(";");
      }
    }
    if (!values.empty()) {
      values.pop_back();
    }
  }
  return values;
}

std::string BasenBmsBle::discharging_warnings_bits_to_string_(const uint8_t mask) {
  std::string values = "";
  if (mask) {
    for (int i = 0; i < DISCHARGING_WARNINGS_SIZE; i++) {
      if (mask & (1 << i)) {
        values.append(DISCHARGING_WARNINGS[i]);
        values.append(";");
      }
    }
    if (!values.empty()) {
      values.pop_back();
    }
  }
  return values;
}

}  // namespace basen_bms_ble
}  // namespace esphome
