#include <Arduino.h>
#include <SPI.h>
#include <airSpeed.h>

uint16_t AirSpeed_Sensor_Pair::read_airSpeed_adc() {
    uint16_t data;
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), LOW);  // LOW to enable
    data = SPI.transfer16(0x0000);
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), HIGH);  // HIGH to disable
    SPI.endTransaction();
    return data;
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
    uint16_t counts = read_airSpeed_adc();
    float pressure = pressure_from_counts(counts);
    return pressure;
}