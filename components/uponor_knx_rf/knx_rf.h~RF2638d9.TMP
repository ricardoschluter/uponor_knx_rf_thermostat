// components/uponor_knx_rf/knxrf.h
#pragma once
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
	namespace uponor_knx_rf {

		class UponorKnxRF : public Component, public spi::SPIDevice {
		public:
			void set_gdo0_pin(GPIOPin* p) { gdo0_pin_ = p; }
			void set_gdo2_pin(GPIOPin* p) { gdo2_pin_ = p; }
			void set_rst_pin(GPIOPin* p) { rst_pin_ = p; }
			void set_frequency(uint32_t f) { frequency_ = f; }

			void setup() override;
			void loop() override;

		protected:
			GPIOPin* gdo0_pin_{ nullptr };
			GPIOPin* gdo2_pin_{ nullptr };
			GPIOPin* rst_pin_{ nullptr };
			uint32_t frequency_{ 868300000 };
			std::unique_ptr<Module> module_;
			std::unique_ptr<CC1101> radio_;
		};

	}  // namespace uponor_knx_rf
}  // namespace esphome
