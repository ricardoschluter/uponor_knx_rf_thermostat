# my_components/knx_rf/__init__.py

import esphome.config_validation as cv
import esphome.codegen as cg
from esphome.const import CONF_ID

# 1) Define the namespace and component class
knx_rf_ns = cg.esphome_ns.namespace('knx_rf')
KNXRFGateway = knx_rf_ns.class_(
    'KNXRFGateway',      # must match the C++ class name
    cg.Component,        # base class for ESPHome components
)

# 2) Configuration schema: only an `id` is needed at top level
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(KNXRFGateway),
}).extend(cv.COMPONENT_SCHEMA)  # includes common component options

async def to_code(config):
    # 3) Create the C++ object
    var = cg.new_Pvariable(config[CONF_ID])
    cg.add_library(
      name="SmartRC-CC1101-Driver-Lib",
      repository="https://github.com/LSatan/SmartRC-CC1101-Driver-Lib.git")
    # 4) Register it so ESPHome calls setup(), loop(), etc.
    await cg.register_component(var, config)
    return var
