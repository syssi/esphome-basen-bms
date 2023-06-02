# esphome-basen-bms

![GitHub actions](https://github.com/syssi/esphome-basen-bms/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-basen-bms)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-basen-bms)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-basen-bms)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to monitor a Basen Battery Management System (Basen-BMS) via BLE

The Basen BMS is probably a rebrand of the VIP-BMS, EE-BMS, Mabru-BMS or Roamer BMS.

Credits: Thanks to [@mager33](https://github.com/mager33) for heavily testing the early beta versions.

## Supported devices

* BASEN 24V 100Ah Bluetooth (BG-24100, reported by [@mager33](https://github.com/syssi/esphome-basen-bms/issues/5#issuecomment-1445179807))

## Untested devices

* BASEN 24V 200Ah Bluetooth (BG-24200)
* BASEN 24V 230Ah Bluetooth (BG-24230)

## Requirements

* [ESPHome 2022.11.0 or higher](https://github.com/esphome/esphome/releases).
* Generic ESP32 board

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-basen-bms@main
```

or just use the `esp32-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

# Clone this external component
git clone https://github.com/syssi/esphome-basen-bms.git
cd esphome-basen-bms

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Validate the configuration, create a binary, upload it, and start logs
esphome run esp32-example.yaml

```

## Example response all sensors enabled

```
[basen_bms_ble:284]: Status frame (28+4 bytes):
[basen_bms_ble:285]:   3A.16.2A.18.03.E5.FF.FF.06.64.00.00.12.14.19.19.35.3D.00.00.80.80.00.00.0E.02.00.00 (28)
[sensor:127]: 'basen-bms-ble current': Sending state -6.90900 A with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble total voltage': Sending state 25.60600 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble temperature 1': Sending state 18.00000 °C with 0 decimals of accuracy
[sensor:127]: 'basen-bms-ble temperature 2': Sending state 20.00000 °C with 0 decimals of accuracy
[sensor:127]: 'basen-bms-ble temperature 3': Sending state 25.00000 °C with 0 decimals of accuracy
[sensor:127]: 'basen-bms-ble temperature 4': Sending state 25.00000 °C with 0 decimals of accuracy
[sensor:127]: 'basen-bms-ble capacity remaining': Sending state 15.66900 Ah with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble charging states bitmask': Sending state 128.00000  with 0 decimals of accuracy
[text_sensor:016]: 'basen-bms-ble charging states': Received new state Charging MOS (CHG)
[text_sensor:067]: 'basen-bms-ble charging states': Sending state 'Charging MOS (CHG)'
[binary_sensor:036]: 'basen-bms-ble charging': Sending state ON
[switch:056]: 'basen-bms-ble charging': Sending state ON
[sensor:127]: 'basen-bms-ble discharging states bitmask': Sending state 128.00000  with 0 decimals of accuracy
[text_sensor:016]: 'basen-bms-ble discharging states': Received new state Discharging MOS (DSG)
[text_sensor:067]: 'basen-bms-ble discharging states': Sending state 'Discharging MOS (DSG)'
[binary_sensor:036]: 'basen-bms-ble discharging': Sending state ON
[switch:056]: 'basen-bms-ble discharging': Sending state ON
[sensor:127]: 'basen-bms-ble charging warnings bitmask': Sending state 0.00000  with 0 decimals of accuracy
[text_sensor:016]: 'basen-bms-ble charging warnings': Received new state 
[text_sensor:067]: 'basen-bms-ble charging warnings': Sending state ''
[sensor:127]: 'basen-bms-ble charging warnings bitmask': Sending state 0.00000  with 0 decimals of accuracy
[text_sensor:016]: 'basen-bms-ble charging warnings': Received new state 
[text_sensor:067]: 'basen-bms-ble charging warnings': Sending state ''
[sensor:127]: 'basen-bms-ble state of charge': Sending state 14.00000 % with 0 decimals of accuracy
[basen_bms_ble:349]: General info frame (28+4 bytes):
[basen_bms_ble:350]:   3A.16.2B.18.A0.86.01.00.00.64.00.00.91.A0.01.00.00.00.00.00.30.75.00.00.71.53.07.00 (28)
[sensor:127]: 'basen-bms-ble nominal capacity': Sending state 100.00001 Ah with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble nominal voltage': Sending state 25.60000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble real capacity': Sending state 106.64101 Ah with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble serial number': Sending state 0.00000  with 0 decimals of accuracy
[text_sensor:016]: 'basen-bms-ble manufacturing date': Received new state 2021.11.17
[text_sensor:067]: 'basen-bms-ble manufacturing date': Sending state '2021.11.17'
[sensor:127]: 'basen-bms-ble charging cycles': Sending state 7.00000  with 0 decimals of accuracy
[basen_bms_ble:402]: Cell voltages frame (chunk 0, 28+4 bytes):
[basen_bms_ble:403]:   3A.16.24.18.96.0C.97.0C.98.0C.96.0C.96.0C.98.0C.98.0C.97.0C.00.00.00.00.00.00.00.00 (28)
[sensor:127]: 'basen-bms-ble cell voltage 1': Sending state 3.22200 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 2': Sending state 3.22300 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 3': Sending state 3.22400 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 4': Sending state 3.22200 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 5': Sending state 3.22200 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 6': Sending state 3.22400 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 7': Sending state 3.22400 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 8': Sending state 3.22300 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 9': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 10': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 11': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 12': Sending state 0.00000 V with 3 decimals of accuracy
[basen_bms_ble:402]: Cell voltages frame (chunk 1, 28+4 bytes):
[basen_bms_ble:403]:   3A.16.25.18.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00.00 (28)
[sensor:127]: 'basen-bms-ble cell voltage 13': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 14': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 15': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 16': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 17': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 18': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 19': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 20': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 21': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 22': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 23': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble cell voltage 24': Sending state 0.00000 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble min cell voltage': Sending state 3.22200 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble max cell voltage': Sending state 3.22400 V with 3 decimals of accuracy
[sensor:127]: 'basen-bms-ble max voltage cell': Sending state 3.00000  with 0 decimals of accuracy
[sensor:127]: 'basen-bms-ble min voltage cell': Sending state 1.00000  with 0 decimals of accuracy
[sensor:127]: 'basen-bms-ble delta cell voltage': Sending state 0.00200 V with 3 decimals of accuracy
[basen_bms_ble:451]: Balancing frame (23+4 bytes):
[basen_bms_ble:452]:   3A.16.FE.13.00.F9.0F.2C.80.80.00.00.80.00.00.00.00.00.00.02.76.53.61 (23)
```

## Protocol

See [docs/protocol-design.md](docs/protocol-design.md).

## Known issues

None.

## Debugging

If this component doesn't work out of the box for your device please update your configuration to increase the log level to see details about the BLE communication and incoming traffic:

```
logger:
  level: VERY_VERBOSE
  logs:
    esp32_ble: DEBUG
    esp32_ble_tracker: VERY_VERBOSE
    basen_bms_ble: VERY_VERBOSE
    scheduler: DEBUG
    component: DEBUG
    sensor: DEBUG
    mqtt: INFO
    mqtt.idf: INFO
    mqtt.component: INFO
    mqtt.sensor: INFO
    mqtt.switch: INFO
    api.service: INFO
    api: INFO
    api: INFO
```

## References

None.
