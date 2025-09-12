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

CONF_SPI_SCK  = "spi_sck_pin"
CONF_SPI_MISO = "spi_miso_pin"
CONF_SPI_MOSI = "spi_mosi_pin"

CONFIG_SCHEMA = (
    cv.Schema({
        cv.GenerateID(): cv.declare_id(UponorKnxRF),
        cv.Required("gdo0_pin"): pins.gpio_input_pin_schema,
        cv.Required("gdo2_pin"): pins.gpio_input_pin_schema,
        cv.Optional("rst_pin"):  pins.gpio_output_pin_schema,
        cv.Optional("frequency", default=868_300_000): cv.frequency,
        cv.Optional("thermostats", default=[]): cv.ensure_list(cv.HexInt),

        # NEW (optional) raw SPI pins for RadioLib init
        cv.Optional(CONF_SPI_SCK):  pins.gpio_pin,   # returns dict with 'number'
        cv.Optional(CONF_SPI_MISO): pins.gpio_pin,
        cv.Optional(CONF_SPI_MOSI): pins.gpio_pin,
    })
    .extend(cv.COMPONENT_SCHEMA)
    .extend(spi.spi_device_schema())
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await spi.register_spi_device(var, config)

    g0 = await cg.gpio_pin_expression(config["gdo0_pin"])
    g2 = await cg.gpio_pin_expression(config["gdo2_pin"])
    cg.add(var.set_gdo0_pin(g0))
    cg.add(var.set_gdo2_pin(g2))

    if "rst_pin" in config:
        rst = await cg.gpio_pin_expression(config["rst_pin"])
        cg.add(var.set_rst_pin(rst))

    cg.add(var.set_frequency(config["frequency"]))
    for tid in config.get("thermostats", []):
        cg.add(var.add_thermostat_id(tid))

    # Pass raw pin numbers for SPI if provided
    if "spi_sck_pin" in config:
        cg.add(var.set_spi_sck_pin(config["spi_sck_pin"]["number"]))
    if "spi_miso_pin" in config:
        cg.add(var.set_spi_miso_pin(config["spi_miso_pin"]["number"]))
    if "spi_mosi_pin" in config:
        cg.add(var.set_spi_mosi_pin(config["spi_mosi_pin"]["number"]))

    cg.add_library(
        name="RadioLib",
        repository="https://github.com/jgromes/RadioLib",
        version="7.2.1",
    )