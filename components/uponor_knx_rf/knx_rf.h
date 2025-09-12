// components/uponor_knx_rf/knxrf.h
#pragma once
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
	namespace uponor_knx_rf {

		class UponorKnxRF : public Component, public spi::SPIDevice {
		public:
			void set_cs_pin(GPIOPin* p) { cs_pin_ = p; }
			void set_gdo0_pin(GPIOPin* p) { gdo0_pin_ = p; }
			void set_gdo2_pin(GPIOPin* p) { gdo2_pin_ = p; }
			void set_rst_pin(GPIOPin* p) { rst_pin_ = p; }

			void setup() override;
			void loop() override;

		protected:
			GPIOPin* cs_pin_{ nullptr };
			GPIOPin* gdo0_pin_{ nullptr };
			GPIOPin* gdo2_pin_{ nullptr };
			GPIOPin* rst_pin_{ nullptr };
		};

	}  // namespace uponor_knx_rf
}  // namespace esphome
