// components/uponor_knx_rf/knxrf.cpp
#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include "knxrf.h"
#include "esphome/components/spi/spi.h"




namespace esphome {
    namespace uponor_knx_rf {

        static const char* const TAG = "uponor_knx_rf";

        // Helper: RadioLib wants plain pin numbers
        static int pin_or_nc(GPIOPin* p) { return p ? p->get_pin() : RADIOLIB_NC; }

        void UponorKnxRF::setup() {
            ESP_LOGI(TAG, "Init CC1101...");

            if (gdo0_pin_) gdo0_pin_->setup();   // input
            if (gdo2_pin_) gdo2_pin_->setup();   // input
            if (rst_pin_) { rst_pin_->setup(); rst_pin_->digital_write(true); }

            // Pins from YAML / base class
            const int cs_pin_num = this->cs_ ? this->cs_->get_pin() : RADIOLIB_NC;  // from SPIDevice
            const int gdo0_pin_num = pin_or_nc(this->gdo0_pin_);
            const int gdo2_pin_num = pin_or_nc(this->gdo2_pin_);
            const int rst_pin_num = pin_or_nc(this->rst_pin_);

            // Create RadioLib objects as members
            this->module_ = std::make_unique<Module>(cs_pin_num, gdo0_pin_num, gdo2_pin_num, rst_pin_num);
            this->radio_ = std::make_unique<CC1101>(this->module_.get());

            // Initialize and tune
            int16_t st = this->radio_->begin();
            if (st != RADIOLIB_ERR_NONE) {
                ESP_LOGE(TAG, "radio.begin() failed: %d", st);
                mark_failed();   // fails the component so users see it in logs
                return;
            }

            // First-lock params (adjust later if needed)
            this->radio_->setFrequency(868.2999);
            this->radio_->setBitRate(32.73);
            this->radio_->setFrequencyDeviation(47.0);
            this->radio_->setRxBandwidth(270.0);
            this->radio_->setOOK(false);
            this->radio_->setEncoding(RADIOLIB_ENCODING_NRZ);
            this->radio_->setPromiscuousMode(true, false);
            this->radio_->disableSyncWordFiltering();
            this->radio_->setCrcFiltering(false);

            st = this->radio_->startReceive();
            if (st != RADIOLIB_ERR_NONE) {
                ESP_LOGE(TAG, "startReceive() failed: %d", st);
                mark_failed();
                return;
            }

            ESP_LOGI(TAG, "CC1101 ready (CS=%d, GDO0=%d, GDO2=%d, RST=%d)",
                cs_pin_num, gdo0_pin_num, gdo2_pin_num, rst_pin_num);
        }

        void UponorKnxRF::loop() {
            if (!this->radio_) return;

            // Non-blocking: check if a framed packet is ready
            if (this->radio_->getPacketLength(false) > 0) {
                uint8_t buf[64];
                size_t len = sizeof(buf);
                if (this->radio_->readData(buf, len) == RADIOLIB_ERR_NONE) {
                    // Minimal hexdump for debugging
                    char line[3 * sizeof(buf) + 8];
                    size_t p = 0;
                    p += snprintf(line + p, sizeof(line) - p, "PKT ");
                    for (size_t i = 0; i < len && p + 3 < sizeof(line); i++) {
                        p += snprintf(line + p, sizeof(line) - p, "%02X ", buf[i]);
                    }
                    ESP_LOGD(TAG, "%s RSSI=%.1f LQI=%d", line, this->radio_->getRSSI(), this->radio_->getLQI());
                }
                this->radio_->startReceive();  // re-arm RX
            }
        }

    }  // namespace uponor_knx_rf
}  // namespace esphome
