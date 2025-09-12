external_components:
  - source: github://yourname/uponor_knx_rf_thermostat
    components: [uponor_knx_rf]

spi:
  clk_pin: 18
  mosi_pin: 23
  miso_pin: 19

uponor_knx_rf:
  cs_pin: 5
  gdo0_pin: 25
  gdo2_pin: 26
  frequency: 868300000
  thermostats: [0x007402302656, 0x007402302650]
