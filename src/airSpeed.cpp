#include <Arduino.h>
#include <SPI.h>
#include <airSpeed.h>

namespace {
const SPISettings kAirspeedSPISettings(1000000, MSBFIRST, SPI_MODE0);
const uint16_t kMuxSettleTimeUs = 10;
const uint16_t kChipSelectSetupUs = 2;
}

AirSpeed_Sensor_Pair::DualADCCounts AirSpeed_Sensor_Pair::read_airSpeed_adc_pair() {
    uint8_t msb = 0;
    uint8_t mid = 0;
    uint8_t lsb = 0;
    uint32_t raw24 = 0;
    SPI.beginTransaction(kAirspeedSPISettings);
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), LOW);  // LOW to enable
    delayMicroseconds(kChipSelectSetupUs);
    msb = SPI.transfer(0x00);
    mid = SPI.transfer(0x00);
    lsb = SPI.transfer(0x00);
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), HIGH);  // HIGH to disable
    SPI.endTransaction();

    raw24 = (static_cast<uint32_t>(msb) << 16) |
            (static_cast<uint32_t>(mid) << 8) |
            static_cast<uint32_t>(lsb);

    DualADCCounts counts{};
    counts.adc_a_counts = static_cast<uint16_t>((raw24 >> 12) & 0x0FFFU);
    counts.adc_b_counts = static_cast<uint16_t>(raw24 & 0x0FFFU);
    return counts;
}

float AirSpeed_Sensor_Pair::pressure_from_counts(uint16_t counts) {
    float pressure = ((static_cast<float>(counts) - AirSpeedConstants::OUTPUT_MIN) * (AirSpeedConstants::PRESSURE_MAX - AirSpeedConstants::PRESSURE_MIN) / (AirSpeedConstants::OUTPUT_MAX - AirSpeedConstants::OUTPUT_MIN)) + AirSpeedConstants::PRESSURE_MIN;
    return pressure;  // pressure (PSI)
}

void AirSpeed_Sensor_Pair::mux_set_writepins(){
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_A0), A0_);
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_A1), A1_);
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_A2), A2_);
}

// redundant function to get sensor reading
float AirSpeed_Sensor_Pair::update_reading(){
    mux_set_writepins();
    delayMicroseconds(kMuxSettleTimeUs);
    const DualADCCounts counts = read_airSpeed_adc_pair();
    float pressure = pressure_from_counts(counts.adc_a_counts);
    return pressure;
}
