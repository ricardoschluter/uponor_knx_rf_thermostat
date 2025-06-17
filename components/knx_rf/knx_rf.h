#ifndef KNX_RF_GATEWAY_H
#define KNX_RF_GATEWAY_H

#include "esphome.h"
#include <vector>
#include <map>
#include <string>

namespace esphome {
	namespace knx_rf {

		/** Raw KNX frame data structure (you can keep your KNXDATA struct here) */
			struct KNXDATA {
				// Always first: raw buffer & length
				uint8_t  data[200];
				int      length;

				// Group 4-byte aligned fields
				double   temperature;
				uint16_t source_address;
				uint16_t target_address;
				uint16_t sensor_data;
				uint16_t serialNoHighWord;
				uint16_t serialNoLowWord;

				// Now 1-byte fields grouped together
				bool     crcError;
				uint8_t  unidirectional;
				uint8_t  is_group_address;
				uint8_t  max_counter;
				uint8_t  frame_no;
				uint8_t  add_ext_type;
				uint8_t  tpci;
				uint8_t  seq_number;
				uint8_t  apci;

				// Null-terminated ID string
				char     sensor_id[13];
			};

		class KNXRFGateway : public Component {
		public:
			/**
			 * @param ids  List of thermostat IDs (hex strings) to listen for
			 */
			explicit KNXRFGateway(const std::vector<std::string>& ids);

			void setup() override;
			void loop() override;

			/** Called by to_code() to register each Sensor* under its ID */
			void add_sensor(const std::string& id, Sensor* sensor);

		private:
			// Helpers
			KNXDATA parse_(const uint8_t* buffer, int len);
			uint8_t get_knx_byte_(const KNXDATA& knx, int offset);
			double transform_temperature_(uint16_t raw);

			std::vector<std::string> ids_;                ///< IDs from YAML
			std::map<std::string, Sensor*> sensor_map_;   ///< ID → Sensor*
			uint8_t buffer_[400];
		};

	}  // namespace knx_rf
}  // namespace esphome

#endif  // KNX_RF_GATEWAY_H
