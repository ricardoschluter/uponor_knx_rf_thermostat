#include "knx_rf.h"
#include "ELECHOUSE_CC1101_SRC_DRV.h" 
#include "esphome.h"

namespace esphome {
    namespace knx_rf {

        KNXRFGateway::KNXRFGateway(const std::vector<std::string>& ids)
            : ids_(ids) {
        }

        void KNXRFGateway::add_sensor(const std::string& id, Sensor* sensor) {
            sensor_map_[id] = sensor;
        }

        void KNXRFGateway::setup() {
            ESP_LOGD("KNXRFGATEWAY", "CC1101 init…");
            ELECHOUSE_cc1101.Init();
            ELECHOUSE_cc1101.setCCMode(1);          // set config for internal transmission mode.
            ELECHOUSE_cc1101.setModulation(0);      // set modulation mode. 0 = 2-FSK, 1 = GFSK, 2 = ASK/OOK, 3 = 4-FSK, 4 = MSK.
            ELECHOUSE_cc1101.setMHZ(868.3);        // Here you can set your basic frequency. The lib calculates the frequency automatically (default = 433.92).The cc1101 can: 300-348 MHZ, 387-464MHZ and 779-928MHZ. Read More info from datasheet.
            ELECHOUSE_cc1101.setDeviation(47.607422);   // Set the Frequency deviation in kHz. Value from 1.58 to 380.85. Default is 47.60 kHz.
            ELECHOUSE_cc1101.setChannel(0);         // Set the Channelnumber from 0 to 255. Default is cahnnel 0.
            ELECHOUSE_cc1101.setChsp(199.951172);       // The channel spacing is multiplied by the channel number CHAN and added to the base frequency in kHz. Value from 25.39 to 405.45. Default is 199.95 kHz.
            ELECHOUSE_cc1101.setRxBW(270.833333);       // Set the Receive Bandwidth in kHz. Value from 58.03 to 812.50. Default is 812.50 kHz.
            ELECHOUSE_cc1101.setDRate(32.7301);       // Set the Data Rate in kBaud. Value from 0.02 to 1621.83. Default is 99.97 kBaud!
            ELECHOUSE_cc1101.setPA(5);             // Set TxPower. The following settings are possible depending on the frequency band.  (-30  -20  -15  -10  -6    0    5    7    10   11   12) Default is max!
            ELECHOUSE_cc1101.setSyncMode(5);       // Combined sync-word qualifier mode. 0 = No preamble/sync. 1 = 16 sync word bits detected. 2 = 16/16 sync word bits detected. 3 = 30/32 sync word bits detected. 4 = No preamble/sync, carrier-sense above threshold. 5 = 15/16 + carrier-sense above threshold. 6 = 16/16 + carrier-sense above threshold. 7 = 30/32 + carrier-sense above threshold.
            ELECHOUSE_cc1101.setSyncWord(0x76, 0x96); // Set sync word. Must be the same for the transmitter and receiver. (Syncword high, Syncword low)
            ELECHOUSE_cc1101.setAdrChk(0);          // Controls address check configuration of received packages. 0 = No address check. 1 = Address check, no broadcast. 2 = Address check and 0 (0x00) broadcast. 3 = Address check and 0 (0x00) and 255 (0xFF) broadcast.
            ELECHOUSE_cc1101.setAddr(0);            // Address used for packet filtration. Optional broadcast addresses are 0 (0x00) and 255 (0xFF).
            ELECHOUSE_cc1101.setWhiteData(0);       // Turn data whitening on / off. 0 = Whitening off. 1 = Whitening on.
            ELECHOUSE_cc1101.setPktFormat(0);       // Format of RX and TX data. 0 = Normal mode, use FIFOs for RX and TX. 1 = Synchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins. 2 = Random TX mode; sends random data using PN9 generator. Used for test. Works as normal mode, setting 0 (00), in RX. 3 = Asynchronous serial mode, Data in on GDO0 and data out on either of the GDOx pins.
            ELECHOUSE_cc1101.setLengthConfig(0);    // 0 = Fixed packet length mode. 1 = Variable packet length mode. 2 = Infinite packet length mode. 3 = Reserved
            ELECHOUSE_cc1101.setPacketLength(61);    // Indicates the packet length when fixed packet length mode is enabled. If variable packet length mode is used, this value indicates the maximum packet length allowed.
            ELECHOUSE_cc1101.setCrc(0);             // 1 = CRC calculation in TX and CRC check in RX enabled. 0 = CRC disabled for TX and RX.
            ELECHOUSE_cc1101.setCRC_AF(0);          // Enable automatic flush of RX FIFO when CRC is not OK. This requires that only one packet is in the RXIFIFO and that packet length is limited to the RX FIFO size.
            ELECHOUSE_cc1101.setDcFilterOff(0);     // Disable digital DC blocking filter before demodulator. Only for data rates ≤ 250 kBaud The recommended IF frequency changes when the DC blocking is disabled. 1 = Disable (current optimized). 0 = Enable (better sensitivity).
            ELECHOUSE_cc1101.setManchester(0);      // Enables Manchester encoding/decoding. 0 = Disable. 1 = Enable.
            ELECHOUSE_cc1101.setFEC(0);             // Enable Forward Error Correction (FEC) with interleaving for packet payload (Only supported for fixed packet length mode. 0 = Disable. 1 = Enable.
            ELECHOUSE_cc1101.setPQT(0);             // Preamble quality estimator threshold. The preamble quality estimator increases an internal counter by one each time a bit is received that is different from the previous bit, and decreases the counter by 8 each time a bit is received that is the same as the last bit. A threshold of 4∙PQT for this counter is used to gate sync word detection. When PQT=0 a sync word is always accepted.
            ELECHOUSE_cc1101.setAppendStatus(1);    // When enabled, two status bytes will be appended to the payload of the packet. The status bytes contain RSSI and LQI values, as well as CRC OK.
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FIFOTHR, 0x40);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FSCTRL1, 0x08);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FSCTRL0, 0x00);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_MDMCFG1, 0x22);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_DEVIATN, 0x47);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_MCSM1, 0x30);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_BSCFG, 0x6D);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FOCCFG, 0x2E);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_AGCCTRL2, 0x43);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_AGCCTRL1, 0x40);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_AGCCTRL0, 0x91);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FREND1, 0xB6);

            ELECHOUSE_cc1101.SpiWriteReg(CC1101_WORCTRL, 0xFB);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FSCAL3, 0xEF);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FSCAL2, 0x2E);
            ELECHOUSE_cc1101.SpiWriteReg(CC1101_FSCAL1, 0x19);
            ESP_LOGD("KNXRFGATEWAY", "setup done");
        }

        void KNXRFGateway::loop() {
            if (!ELECHOUSE_cc1101.CheckRxFifo(100) || !ELECHOUSE_cc1101.CheckCRC())
                return;

            int len = ELECHOUSE_cc1101.ReceiveData(buffer_);
            auto knx = parse_(buffer_, len);

            if (sensor_map_.empty()) {
                // Listener mode: no sensors registered
                if (knx.target_address == 1 && knx.temperature > 16.0) {
                    ESP_LOGD("KNX_RF", "Get message from: %s  (temp=%.1f °C, tgt=%u)",
                        knx.sensor_id, knx.temperature, knx.target_address);
                }
                return;
            }

            // Normal mode: only publish for configured sensors
            auto it = sensor_map_.find(knx.sensor_id);
            if (it != sensor_map_.end()) {
                it->second->publish_state(knx.temperature);
            }
        }

        KNXDATA KNXRFGateway::parse_(const uint8_t* buf, int len) {
            KNXDATA d;
            d.length = len;
            memcpy(d.data, buf, len);

            // Example offsets (you may need to tweak these):
            constexpr int offset_id = knx_offset_;            // where sensor ID bytes start
            constexpr int offset_temp = knx_offset_ + 4;      // where temperature bytes start
            +constexpr int offset_target = knx_offset_ + 2;    // where target_address lives

            // Build ID string
            snprintf(d.sensor_id, sizeof(d.sensor_id), "%02X%02X%02X%02X",
                buf[offset_id], buf[offset_id + 1],
                buf[offset_id + 2], buf[offset_id + 3]);

            // Parse raw temperature (2 bytes)
            uint16_t raw = (uint16_t(buf[offset_temp]) << 8) | buf[offset_temp + 1];
            d.temperature = transform_temperature_(raw);

            + // Parse target address
                +d.target_address = buf[offset_target];

            return d;
        }

        uint8_t KNXRFGateway::get_knx_byte_(const KNXDATA& knx, int offset) {
            if (offset < 0 || offset >= knx.length) return 0;
            return knx.data[offset];
        }
        
        double KNXRFGateway::transform_temperature_(uint16_t raw) {
            bool negative = raw & 0x800;
            uint16_t mag = raw & 0x7FF;
            double temp = mag * 0.1;
            return negative ? -temp : temp;
        }

    }  // namespace knx_rf
}  // namespace esphome
