# ESPhome component for interfacing Uponor Control System (DEM) KNX RF thermostats

This is my fork for custom ESPhome component for reading information sent by UPONOR T-75, T-37, T-33 and T-54 KNX RF thermostats. It has been tested with ESP32.

It is using CC1101 for a radio interface. For hardware setup see https://github.com/LSatan/SmartRC-CC1101-Driver-Lib.

The component from the original repo has been simplified to only publish temperature values. Please modify the number of sensors in the *configuration.yaml* and in *knx_rf.h* accordingly to your needs.

## Stack Size might need increasing to avoid crashing

**EDIT FILE:**
- *.platformio/packages/framework-arduinoespressif32/cores/esp32/main.cpp*

**CHANGE:**
- *xTaskCreateUniversal(loopTask, "loopTask", ARDUINO_LOOP_STACK_SIZE, NULL, 1, &loopTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);*

**TO:**
- *xTaskCreateUniversal(loopTask, "loopTask", 32768, NULL, 1, &loopTaskHandle, CONFIG_ARDUINO_RUNNING_CORE);*

For further information see: https://github.com/esphome/issues/issues/855

## Initial setup

To find all MAC-addresses of the thermostats in your system, enable debugging:

<pre>
logger:
  level: DEBUG
</pre>
