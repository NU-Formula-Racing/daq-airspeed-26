#include <Arduino.h>
#include <SPI.h>
#include <airSpeed.h>

namespace {
const SPISettings kAirspeedSPISettings(1000000, MSBFIRST, SPI_MODE0);
const uint16_t kMuxSettleTimeUs = 10;
const uint16_t kChipSelectSetupUs = 5;
}

AirSpeed_Sensor_Pair::DualADCCounts AirSpeed_Sensor_Pair::read_airSpeed_adc_pair() {
    uint16_t frame0 = 0;
    uint16_t frame1 = 0;

    SPI.beginTransaction(kAirspeedSPISettings);
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), LOW);  // LOW to enable
    delayMicroseconds(kChipSelectSetupUs);
    frame0 = SPI.transfer16(0x0000);  // First 16 clocks
    frame1 = SPI.transfer16(0x0000);  // Second 16 clocks
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), HIGH);  // HIGH to disable
    SPI.endTransaction();

    DualADCCounts counts{};
    // Each 16-clock frame is 00 + 12-bit data + 00.
    counts.adc_a_counts = static_cast<uint16_t>((frame0 >> 2) & 0x0FFFU);
    counts.adc_b_counts = static_cast<uint16_t>((frame1 >> 2) & 0x0FFFU);
    counts.raw_frame0 = frame0;
    counts.raw_frame1 = frame1;
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
