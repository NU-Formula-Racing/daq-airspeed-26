#include <Arduino.h>
#include <SPI.h>
#include <airSpeed.h>


    // Constructor to set the specific ADC channel pin

// Initialize pins - call this in setup()
void AirSpeed_Sensor_Pair::init() {
    pinMode(adc_pin_num_, INPUT);  // Configure ADC pin as input
    SPI.begin();
}

uint16_t AirSpeed_Sensor_Pair::read_airSpeed_adc() {
    uint16_t data;
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
    digitalWrite(AS_SPI_CS, LOW);  // LOW to enable
    data = SPI.transfer16(0x0000);
    digitalWrite(AS_SPI_CS, HIGH);  // HIGH to disable
    SPI.endTransaction();
    return data;
}

float AirSpeed_Sensor_Pair::pressure_from_counts(uint16_t counts) {
    float pressure = ((static_cast<float>(counts) - AirSpeedConstants::OUTPUT_MIN) * (AirSpeedConstants::PRESSURE_MAX - AirSpeedConstants::PRESSURE_MIN) / (AirSpeedConstants::OUTPUT_MAX - AirSpeedConstants::OUTPUT_MIN)) + AirSpeedConstants::PRESSURE_MIN;
    return pressure;  // pressure (PSI)
}

