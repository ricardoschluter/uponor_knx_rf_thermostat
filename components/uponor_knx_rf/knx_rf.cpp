// knxrf.cpp
#include <Arduino.h>
#include <SPI.h>
#include <RadioLib.h>
#include "knxrf.h"
#include "esphome/core/log.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
    namespace uponor_knx_rf {

        static const char* const TAG = "uponor_knx_rf";

        static int pin_or_nc(GPIOPin* p) { return p ? p->get_pin() : RADIOLIB_NC; }

        void UponorKnxRF::setup() {
            ESP_LOGI(TAG, "Init CC1101...");

            if (gdo0_pin_) gdo0_pin_->setup();
            if (gdo2_pin_) gdo2_pin_->setup();
            if (rst_pin_) { rst_pin_->setup(); rst_pin_->digital_write(true); }

            const int cs_pin_num = this->cs_ ? this->cs_->get_pin() : RADIOLIB_NC;  // from SPIDevice
            const int gdo0_pin_num = pin_or_nc(this->gdo0_pin_);
            const int gdo2_pin_num = pin_or_nc(this->gdo2_pin_);
            const int rst_pin_num = pin_or_nc(this->rst_pin_);

            // If user provided raw SPI pins -> initialize bus and use extended Module(...)
            if (spi_sck_ >= 0 && spi_miso_ >= 0 && spi_mosi_ >= 0) {
                ESP_LOGI(TAG, "Using custom SPI pins SCK=%d MISO=%d MOSI=%d", spi_sck_, spi_miso_, spi_mosi_);
                SPI.begin(spi_sck_, spi_miso_, spi_mosi_, cs_pin_num);
                SPISettings spi_settings(2'000'000, MSBFIRST, SPI_MODE0);  // 2 MHz is plenty for CC1101
                this->module_ = std::make_unique<Module>(cs_pin_num, gdo0_pin_num, gdo2_pin_num, rst_pin_num, SPI, spi_settings);
            }
            else {
                // Default VSPI pins (18/19/23) — RadioLib will call SPI.begin() itself
                this->module_ = std::make_unique<Module>(cs_pin_num, gdo0_pin_num, gdo2_pin_num, rst_pin_num);
            }

            this->radio_ = std::make_unique<CC1101>(this->module_.get());

            int16_t st = this->radio_->begin();
            if (st != RADIOLIB_ERR_NONE) {
                ESP_LOGE(TAG, "radio.begin() failed: %d", st);
                mark_failed();
                return;
            }

            // First-lock parameters (tweak later or make YAML-configurable)
            this->radio_->setFrequency(this->frequency_ / 1e6);  // RadioLib expects MHz double
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
            }
            else {
                ESP_LOGI(TAG, "CC1101 ready (CS=%d GDO0=%d GDO2=%d RST=%d)", cs_pin_num, gdo0_pin_num, gdo2_pin_num, rst_pin_num);
            }
        }

        void UponorKnxRF::loop() {
            if (!this->radio_) return;
            if (this->radio_->getPacketLength(false) > 0) {
                uint8_t buf[64];
                size_t len = sizeof(buf);
                if (this->radio_->readData(buf, len) == RADIOLIB_ERR_NONE) {
                    ESP_LOGD(TAG, "PKT len=%u RSSI=%.1f LQI=%d", (unsigned)len, this->radio_->getRSSI(), this->radio_->getLQI());
                }
                this->radio_->startReceive();
            }
        }

    }  // namespace uponor_knx_rf
}  // namespace esphome
