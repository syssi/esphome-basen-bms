#include <gtest/gtest.h>
#include "common.h"
#include "frames.h"

namespace esphome::basen_bms_ble::testing {

// ── Current ───────────────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, Current) {
  TestableBasenBmsBle bms;
  sensor::Sensor current;
  bms.set_current_sensor(&current);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_NEAR(current.state, -6.909f, 0.001f);
}

// ── Total voltage ─────────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, TotalVoltage) {
  TestableBasenBmsBle bms;
  sensor::Sensor total_voltage;
  bms.set_total_voltage_sensor(&total_voltage);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_NEAR(total_voltage.state, 25.606f, 0.001f);
}

// ── Power ─────────────────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, Power) {
  TestableBasenBmsBle bms;
  sensor::Sensor power, charging_power, discharging_power;
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_NEAR(power.state, -176.91f, 0.5f);
  EXPECT_NEAR(charging_power.state, 0.0f, 0.01f);
  EXPECT_NEAR(discharging_power.state, 176.91f, 0.5f);
}

// ── Temperatures ──────────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, Temperatures) {
  TestableBasenBmsBle bms;
  sensor::Sensor t0, t1, t2, t3;
  bms.set_temperature_sensor(0, &t0);
  bms.set_temperature_sensor(1, &t1);
  bms.set_temperature_sensor(2, &t2);
  bms.set_temperature_sensor(3, &t3);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(t0.state, 18.0f);
  EXPECT_FLOAT_EQ(t1.state, 20.0f);
  EXPECT_FLOAT_EQ(t2.state, 25.0f);
  EXPECT_FLOAT_EQ(t3.state, 25.0f);
}

// ── Capacity remaining ────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, CapacityRemaining) {
  TestableBasenBmsBle bms;
  sensor::Sensor cap;
  bms.set_capacity_remaining_sensor(&cap);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_NEAR(cap.state, 15.669f, 0.001f);
}

// ── Charging states ───────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, ChargingStates) {
  TestableBasenBmsBle bms;
  sensor::Sensor bitmask;
  binary_sensor::BinarySensor charging;
  bms.set_charging_states_bitmask_sensor(&bitmask);
  bms.set_charging_binary_sensor(&charging);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(bitmask.state, 128.0f);
  EXPECT_TRUE(charging.state);
}

// ── Discharging states ────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, DischargingStates) {
  TestableBasenBmsBle bms;
  sensor::Sensor bitmask;
  binary_sensor::BinarySensor discharging;
  bms.set_discharging_states_bitmask_sensor(&bitmask);
  bms.set_discharging_binary_sensor(&discharging);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(bitmask.state, 128.0f);
  EXPECT_TRUE(discharging.state);
}

// ── Warnings ──────────────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, Warnings) {
  TestableBasenBmsBle bms;
  sensor::Sensor charge_warn, discharge_warn;
  bms.set_charging_warnings_bitmask_sensor(&charge_warn);
  bms.set_discharging_warnings_bitmask_sensor(&discharge_warn);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(charge_warn.state, 0.0f);
  EXPECT_FLOAT_EQ(discharge_warn.state, 0.0f);
}

// ── State of charge ───────────────────────────────────────────────────────────

TEST(BasenBmsBleStatusTest, StateOfCharge) {
  TestableBasenBmsBle bms;
  sensor::Sensor soc;
  bms.set_state_of_charge_sensor(&soc);

  bms.on_basen_bms_ble_data(STATUS_FRAME);

  EXPECT_FLOAT_EQ(soc.state, 14.0f);
}

// ── General info ──────────────────────────────────────────────────────────────

TEST(BasenBmsBleGeneralInfoTest, Capacities) {
  TestableBasenBmsBle bms;
  sensor::Sensor nominal_cap, nominal_volt, real_cap;
  bms.set_nominal_capacity_sensor(&nominal_cap);
  bms.set_nominal_voltage_sensor(&nominal_volt);
  bms.set_real_capacity_sensor(&real_cap);

  bms.on_basen_bms_ble_data(GENERAL_INFO_FRAME);

  EXPECT_NEAR(nominal_cap.state, 100.0f, 0.001f);
  EXPECT_NEAR(nominal_volt.state, 25.6f, 0.001f);
  EXPECT_NEAR(real_cap.state, 106.641f, 0.001f);
}

TEST(BasenBmsBleGeneralInfoTest, SerialAndCycles) {
  TestableBasenBmsBle bms;
  sensor::Sensor serial, cycles;
  bms.set_serial_number_sensor(&serial);
  bms.set_charging_cycles_sensor(&cycles);

  bms.on_basen_bms_ble_data(GENERAL_INFO_FRAME);

  EXPECT_FLOAT_EQ(serial.state, 0.0f);
  EXPECT_FLOAT_EQ(cycles.state, 7.0f);
}

TEST(BasenBmsBleGeneralInfoTest, ManufacturingDate) {
  TestableBasenBmsBle bms;
  text_sensor::TextSensor date;
  bms.set_manufacturing_date_text_sensor(&date);

  bms.on_basen_bms_ble_data(GENERAL_INFO_FRAME);

  EXPECT_EQ(date.state, "2021.11.17");
}

// ── Cell voltages ─────────────────────────────────────────────────────────────

TEST(BasenBmsBleCellVoltagesTest, IndividualCells) {
  TestableBasenBmsBle bms;
  sensor::Sensor cells[8];
  for (int i = 0; i < 8; i++)
    bms.set_cell_voltage_sensor(i, &cells[i]);

  bms.on_basen_bms_ble_data(CELL_VOLTAGES_1_12_FRAME);

  EXPECT_NEAR(cells[0].state, 3.222f, 0.001f);
  EXPECT_NEAR(cells[1].state, 3.223f, 0.001f);
  EXPECT_NEAR(cells[2].state, 3.224f, 0.001f);
  EXPECT_NEAR(cells[3].state, 3.222f, 0.001f);
  EXPECT_NEAR(cells[4].state, 3.222f, 0.001f);
  EXPECT_NEAR(cells[5].state, 3.224f, 0.001f);
  EXPECT_NEAR(cells[6].state, 3.224f, 0.001f);
  EXPECT_NEAR(cells[7].state, 3.223f, 0.001f);
}

TEST(BasenBmsBleCellVoltagesTest, AggregatedStats) {
  TestableBasenBmsBle bms;
  sensor::Sensor min_v, max_v, min_cell, max_cell, delta, avg;
  bms.set_min_cell_voltage_sensor(&min_v);
  bms.set_max_cell_voltage_sensor(&max_v);
  bms.set_min_voltage_cell_sensor(&min_cell);
  bms.set_max_voltage_cell_sensor(&max_cell);
  bms.set_delta_cell_voltage_sensor(&delta);
  bms.set_average_cell_voltage_sensor(&avg);

  bms.on_basen_bms_ble_data(CELL_VOLTAGES_1_12_FRAME);
  bms.on_basen_bms_ble_data(CELL_VOLTAGES_13_24_FRAME);

  EXPECT_NEAR(min_v.state, 3.222f, 0.001f);
  EXPECT_NEAR(max_v.state, 3.224f, 0.001f);
  EXPECT_FLOAT_EQ(min_cell.state, 1.0f);
  EXPECT_FLOAT_EQ(max_cell.state, 3.0f);
  EXPECT_NEAR(delta.state, 0.002f, 0.001f);
  EXPECT_NEAR(avg.state, 3.223f, 0.001f);
}

// ── Balancing ─────────────────────────────────────────────────────────────────

TEST(BasenBmsBleBaleancingTest, BalancingCells) {
  TestableBasenBmsBle bms;
  binary_sensor::BinarySensor balancing;
  sensor::Sensor bitmask;
  text_sensor::TextSensor cells;
  bms.set_balancing_binary_sensor(&balancing);
  bms.set_balancing_cells_bitmask_sensor(&bitmask);
  bms.set_balancing_cells_text_sensor(&cells);

  bms.on_basen_bms_ble_data(BALANCING_FRAME);

  EXPECT_TRUE(balancing.state);
  EXPECT_FLOAT_EQ(bitmask.state, 32896.0f);
  EXPECT_EQ(cells.state, "8, 16");
}

// ── Online status tracker ─────────────────────────────────────────────────────

TEST(BasenBmsBleOnlineStatusTrackerTest, ReachesThreshold) {
  TestableBasenBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();

  EXPECT_FALSE(online_status.state);
}

TEST(BasenBmsBleOnlineStatusTrackerTest, DoesNotRepeatAfterThreshold) {
  TestableBasenBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  EXPECT_EQ(bms.get_no_response_count(), 11);

  bms.track_online_status_();
  EXPECT_EQ(bms.get_no_response_count(), 11);
}

TEST(BasenBmsBleOnlineStatusTrackerTest, ResetRestoresOnlineStatus) {
  TestableBasenBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  EXPECT_FALSE(online_status.state);

  bms.reset_online_status_tracker_();
  EXPECT_TRUE(online_status.state);
  EXPECT_EQ(bms.get_no_response_count(), 0);
}

TEST(BasenBmsBleOnlineStatusTrackerTest, CanRetriggerAfterReset) {
  TestableBasenBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  bms.reset_online_status_tracker_();

  for (int i = 0; i < 10; i++)
    bms.track_online_status_();
  EXPECT_FALSE(online_status.state);
}

TEST(BasenBmsBleOnlineStatusTrackerTest, ValidFrameResetsCounter) {
  TestableBasenBmsBle bms;

  for (int i = 0; i < 5; i++)
    bms.track_online_status_();
  EXPECT_EQ(bms.get_no_response_count(), 5);

  bms.on_basen_bms_ble_data(STATUS_FRAME);
  EXPECT_EQ(bms.get_no_response_count(), 0);
}

TEST(BasenBmsBleOnlineStatusTrackerTest, NotYetAtThresholdStaysOnline) {
  TestableBasenBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  online_status.publish_state(true);
  for (int i = 0; i < 9; i++)
    bms.track_online_status_();

  EXPECT_TRUE(online_status.state);
}

// ── publish_device_unavailable_ ───────────────────────────────────────────────

TEST(BasenBmsBlePublishDeviceUnavailableTest, SetsOnlineStatusFalse) {
  TestableBasenBmsBle bms;
  binary_sensor::BinarySensor online_status;
  bms.set_online_status_binary_sensor(&online_status);

  online_status.publish_state(true);
  bms.publish_device_unavailable_();

  EXPECT_FALSE(online_status.state);
}

TEST(BasenBmsBlePublishDeviceUnavailableTest, SetsNumericSensorsToNAN) {
  TestableBasenBmsBle bms;
  sensor::Sensor total_voltage, current, power, charging_power, discharging_power;
  sensor::Sensor capacity_remaining, state_of_charge, nominal_capacity, nominal_voltage;
  sensor::Sensor real_capacity, charging_cycles;
  bms.set_total_voltage_sensor(&total_voltage);
  bms.set_current_sensor(&current);
  bms.set_power_sensor(&power);
  bms.set_charging_power_sensor(&charging_power);
  bms.set_discharging_power_sensor(&discharging_power);
  bms.set_capacity_remaining_sensor(&capacity_remaining);
  bms.set_state_of_charge_sensor(&state_of_charge);
  bms.set_nominal_capacity_sensor(&nominal_capacity);
  bms.set_nominal_voltage_sensor(&nominal_voltage);
  bms.set_real_capacity_sensor(&real_capacity);
  bms.set_charging_cycles_sensor(&charging_cycles);

  bms.publish_device_unavailable_();

  EXPECT_TRUE(std::isnan(total_voltage.state));
  EXPECT_TRUE(std::isnan(current.state));
  EXPECT_TRUE(std::isnan(power.state));
  EXPECT_TRUE(std::isnan(charging_power.state));
  EXPECT_TRUE(std::isnan(discharging_power.state));
  EXPECT_TRUE(std::isnan(capacity_remaining.state));
  EXPECT_TRUE(std::isnan(state_of_charge.state));
  EXPECT_TRUE(std::isnan(nominal_capacity.state));
  EXPECT_TRUE(std::isnan(nominal_voltage.state));
  EXPECT_TRUE(std::isnan(real_capacity.state));
  EXPECT_TRUE(std::isnan(charging_cycles.state));
}

TEST(BasenBmsBlePublishDeviceUnavailableTest, SetsCellAndTempSensorsToNAN) {
  TestableBasenBmsBle bms;
  sensor::Sensor cell0, cell1, temp0, temp1;
  bms.set_cell_voltage_sensor(0, &cell0);
  bms.set_cell_voltage_sensor(1, &cell1);
  bms.set_temperature_sensor(0, &temp0);
  bms.set_temperature_sensor(1, &temp1);

  bms.publish_device_unavailable_();

  EXPECT_TRUE(std::isnan(cell0.state));
  EXPECT_TRUE(std::isnan(cell1.state));
  EXPECT_TRUE(std::isnan(temp0.state));
  EXPECT_TRUE(std::isnan(temp1.state));
}

TEST(BasenBmsBlePublishDeviceUnavailableTest, SetsDynamicTextSensorsToOffline) {
  TestableBasenBmsBle bms;
  text_sensor::TextSensor charging_states, discharging_states, charging_warnings;
  text_sensor::TextSensor discharging_warnings, balancing_cells;
  bms.set_charging_states_text_sensor(&charging_states);
  bms.set_discharging_states_text_sensor(&discharging_states);
  bms.set_charging_warnings_text_sensor(&charging_warnings);
  bms.set_discharging_warnings_text_sensor(&discharging_warnings);
  bms.set_balancing_cells_text_sensor(&balancing_cells);

  bms.publish_device_unavailable_();

  EXPECT_EQ(charging_states.state, "Offline");
  EXPECT_EQ(discharging_states.state, "Offline");
  EXPECT_EQ(charging_warnings.state, "Offline");
  EXPECT_EQ(discharging_warnings.state, "Offline");
  EXPECT_EQ(balancing_cells.state, "Offline");
}

TEST(BasenBmsBlePublishDeviceUnavailableTest, LeavesStaticTextSensorsUnchanged) {
  TestableBasenBmsBle bms;
  text_sensor::TextSensor manufacturing_date;
  manufacturing_date.publish_state("2021.11.17");
  bms.set_manufacturing_date_text_sensor(&manufacturing_date);

  bms.publish_device_unavailable_();

  EXPECT_EQ(manufacturing_date.state, "2021.11.17");
}

TEST(BasenBmsBlePublishDeviceUnavailableTest, NullSensorsDoNotCrash) {
  TestableBasenBmsBle bms;

  EXPECT_NO_FATAL_FAILURE(bms.publish_device_unavailable_());
}

// ── Null sensors do not crash ─────────────────────────────────────────────────

TEST(BasenBmsBleSafetyTest, NullSensorsDoNotCrash) {
  TestableBasenBmsBle bms;

  EXPECT_NO_FATAL_FAILURE(bms.on_basen_bms_ble_data(STATUS_FRAME));
  EXPECT_NO_FATAL_FAILURE(bms.on_basen_bms_ble_data(GENERAL_INFO_FRAME));
  EXPECT_NO_FATAL_FAILURE(bms.on_basen_bms_ble_data(CELL_VOLTAGES_1_12_FRAME));
  EXPECT_NO_FATAL_FAILURE(bms.on_basen_bms_ble_data(CELL_VOLTAGES_13_24_FRAME));
  EXPECT_NO_FATAL_FAILURE(bms.on_basen_bms_ble_data(BALANCING_FRAME));
}

}  // namespace esphome::basen_bms_ble::testing
