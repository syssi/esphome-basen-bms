import esphome.codegen as cg
from esphome.components import ble_client
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]

AUTO_LOAD = ["binary_sensor", "sensor", "switch", "text_sensor"]
MULTI_CONF = True

CONF_BASEN_BMS_BLE_ID = "basen_bms_ble_id"

basen_bms_ble_ns = cg.esphome_ns.namespace("basen_bms_ble")
BasenBmsBle = basen_bms_ble_ns.class_(
    "BasenBmsBle", ble_client.BLEClientNode, cg.PollingComponent
)

BASEN_BMS_BLE_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_BASEN_BMS_BLE_ID): cv.use_id(BasenBmsBle),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(BasenBmsBle),
        }
    )
    .extend(ble_client.BLE_CLIENT_SCHEMA)
    .extend(cv.polling_component_schema("10s"))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ble_client.register_ble_node(var, config)
