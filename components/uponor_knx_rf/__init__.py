import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi
from esphome import pins 
from esphome.const import CONF_ID, CONF_CS_PIN  

CODEOWNERS = ["@ricardoschluter"]
DEPENDENCIES = ["spi"]
AUTO_LOAD = ["logger"]
MULTI_CONF = False

ns = cg.esphome_ns.namespace("uponor_knx_rf")
UponorKnxRF = ns.class_("UponorKnxRF", cg.Component, spi.SPIDevice)

CONF_GDO0_PIN = "gdo0_pin"
CONF_GDO2_PIN = "gdo2_pin"
CONF_RST_PIN  = "rst_pin"
CONF_FREQUENCY = "frequency"
CONF_THERMOSTATS = "thermostats"


CONFIG_SCHEMA = (
    cv.Schema({
        cv.GenerateID(): cv.declare_id(UponorKnxRF),

        cv.Required(CONF_GDO0_PIN): pins.gpio_input_pin_schema,
        cv.Required(CONF_GDO2_PIN): pins.gpio_input_pin_schema,
        cv.Optional(CONF_RST_PIN):  pins.gpio_output_pin_schema,
        cv.Optional(CONF_FREQUENCY, default=868_300_000): cv.frequency,
        cv.Optional(CONF_THERMOSTATS, default=[]): cv.ensure_list(cv.HexInt),
    })
    .extend(cv.COMPONENT_SCHEMA)
    .extend(spi.spi_device_schema())
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)  # this wires up cs_pin for you

    g0 = await cg.gpio_pin_expression(config["gdo0_pin"])
    g2 = await cg.gpio_pin_expression(config["gdo2_pin"])
    cg.add(var.set_gdo0_pin(g0))
    cg.add(var.set_gdo2_pin(g2))

    if CONF_RST_PIN in config:
        rst = await cg.gpio_pin_expression(config[CONF_RST_PIN])
        cg.add(var.set_rst_pin(rst))

    cg.add(var.set_frequency(config["frequency"]))
    for tid in config.get("thermostats", []):
        cg.add(var.add_thermostat_id(tid))

    cg.add_library(
        name="RadioLib",
        repository="https://github.com/jgromes/RadioLib",
        version="7.2.1",
    )
