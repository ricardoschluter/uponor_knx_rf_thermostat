// components/uponor_knx_rf/knxrf.cpp
#include "knxrf.h"
#include <RadioLib.h>

namespace esphome {
    namespace uponor_knx_rf {

        // RadioLib needs the raw pin numbers
        static int pin_or_nc(GPIOPin* p) { return p ? p->get_pin() : RADIOLIB_NC; }

        void UponorKnxRF::setup() {
            // Configure pins
            if (cs_pin_)   cs_pin_->setup();    // output; SPIDevice will toggle as needed
            if (gdo0_pin_) gdo0_pin_->setup();  // input
            if (gdo2_pin_) gdo2_pin_->setup();  // input
            if (rst_pin_) { rst_pin_->setup(); rst_pin_->digital_write(true); }

            // Create the RadioLib module with USER-set pins
            Module* m = new Module(
                pin_or_nc(cs_pin_),     // CS
                pin_or_nc(gdo0_pin_),   // GDO0
                pin_or_nc(gdo2_pin_),   // GDO2
                pin_or_nc(rst_pin_)     // RST (or NC)
            );
            static CC1101 radio(m);

            // Init & tune (example values; adjust later)
            radio.begin();
            radio.setFrequency(868.2999);
            radio.setBitRate(32.73);
            radio.setFrequencyDeviation(47.0);
            radio.setRxBandwidth(270.0);
            radio.setOOK(false);
            radio.setEncoding(RADIOLIB_ENCODING_NRZ);
            radio.setPromiscuousMode(true, false);
            radio.disableSyncWordFiltering();
            radio.setCrcFiltering(false);
            radio.startReceive();

            // (Store `radio` somewhere global/singleton or as a member if you refactor)
        }

        void UponorKnxRF::loop() {
            // … read frames and publish …
        }

    }  // namespace uponor_knx_rf
}  // namespace esphome
