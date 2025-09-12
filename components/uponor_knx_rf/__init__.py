# components/uponor_knx_rf/__init__.py
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi
from esphome.const import CONF_ID

CODEOWNERS = ["@yourhandle"]
DEPENDENCIES = ["spi"]
AUTO_LOAD = ["logger"]

ns = cg.namespace("esphome").namespace("uponor_knx_rf")
UponorKnxRF = ns.class_("UponorKnxRF", cg.Component, spi.SPIDevice)

CONF_CS_PIN   = "cs_pin"
CONF_GDO0_PIN = "gdo0_pin"
CONF_GDO2_PIN = "gdo2_pin"
CONF_RST_PIN  = "rst_pin"  # optional

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(UponorKnxRF),

    # SPI device select (validates it’s on the configured SPI bus)
    cv.Required(CONF_CS_PIN): cv.spi_cs_pin,

    # GDO lines as inputs (internal schema enables pullup/interrupt options)
    cv.Required(CONF_GDO0_PIN): cv.internal_gpio_input_pin_schema,
    cv.Required(CONF_GDO2_PIN): cv.internal_gpio_input_pin_schema,

    # Optional reset pin (many CC1101 boards don’t break this out)
    cv.Optional(CONF_RST_PIN): cv.internal_gpio_output_pin_schema,

    # (… your other options …)
}).extend(cv.COMPONENT_SCHEMA).extend(spi.spi_device_schema())

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)  # binds to the YAML `spi:` bus

    cs = await cg.gpio_pin_expression(config[CONF_CS_PIN])
    cg.add(var.set_cs_pin(cs))

    g0 = await cg.gpio_pin_expression(config[CONF_GDO0_PIN])
    cg.add(var.set_gdo0_pin(g0))

    g2 = await cg.gpio_pin_expression(config[CONF_GDO2_PIN])
    cg.add(var.set_gdo2_pin(g2))

    if CONF_RST_PIN in config:
        rst = await cg.gpio_pin_expression(config[CONF_RST_PIN])
        cg.add(var.set_rst_pin(rst))

    # If you use RadioLib:
    cg.add_library(
        name="RadioLib",
        repository="https://github.com/jgromes/RadioLib",
        version="7.2.1",
    )
