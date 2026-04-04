"""Schema structure tests for basen_bms_ble ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.basen_bms_ble as hub  # noqa: E402
from components.basen_bms_ble import (  # noqa: E402
    binary_sensor,
    sensor,
    switch,  # noqa: E402
    text_sensor,
)


class TestHubConstants:
    def test_conf_basen_bms_ble_id_defined(self):
        assert hub.CONF_BASEN_BMS_BLE_ID == "basen_bms_ble_id"


class TestSensorLists:
    def test_cells_count(self):
        assert len(sensor.CELLS) == 34

    def test_cells_naming(self):
        assert sensor.CELLS[0] == "cell_voltage_1"
        assert sensor.CELLS[33] == "cell_voltage_34"
        for i, key in enumerate(sensor.CELLS, 1):
            assert key == f"cell_voltage_{i}"

    def test_temperatures_count(self):
        assert len(sensor.TEMPERATURES) == 4

    def test_temperatures_naming(self):
        assert sensor.TEMPERATURES[0] == "temperature_1"
        assert sensor.TEMPERATURES[3] == "temperature_4"
        for i, key in enumerate(sensor.TEMPERATURES, 1):
            assert key == f"temperature_{i}"

    def test_sensor_defs_completeness(self):
        assert sensor.CONF_TOTAL_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_STATE_OF_CHARGE in sensor.SENSOR_DEFS
        assert sensor.CONF_MIN_CELL_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_MAX_CELL_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_DELTA_CELL_VOLTAGE in sensor.SENSOR_DEFS
        assert sensor.CONF_AVERAGE_CELL_VOLTAGE in sensor.SENSOR_DEFS
        assert len(sensor.SENSOR_DEFS) == 23

    def test_no_cell_keys_in_sensor_defs(self):
        for key in sensor.SENSOR_DEFS:
            assert key not in sensor.CELLS
            assert key not in sensor.TEMPERATURES


class TestBinarySensorDefs:
    def test_binary_sensor_defs_completeness(self):
        assert binary_sensor.CONF_BALANCING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_CHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert binary_sensor.CONF_DISCHARGING in binary_sensor.BINARY_SENSOR_DEFS
        assert len(binary_sensor.BINARY_SENSOR_DEFS) == 3


class TestTextSensors:
    def test_text_sensors_list(self):
        assert text_sensor.CONF_CHARGING_STATES in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_DISCHARGING_STATES in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_CHARGING_WARNINGS in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_DISCHARGING_WARNINGS in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_MANUFACTURING_DATE in text_sensor.TEXT_SENSORS
        assert text_sensor.CONF_BALANCING_CELLS in text_sensor.TEXT_SENSORS
        assert len(text_sensor.TEXT_SENSORS) == 6


class TestSwitchConstants:
    def test_switches_dict(self):
        assert switch.CONF_CHARGING in switch.SWITCHES
        assert switch.CONF_DISCHARGING in switch.SWITCHES
        assert len(switch.SWITCHES) == 2
