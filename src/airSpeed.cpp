#ifndef __AIRSPEED_H__
#define __AIRSPEED_H__

#include <Arduino.h>
#include <SPI.h>

class AirSpeed_Sensor_Pair {
public: 
    // Constructor to set the specific ADC channel pin
    AirSpeed_Sensor_Pair(int adc_pin_num) : adc_pin_num_(adc_pin_num) {}

    // Initialize pins - call this in setup()
    void init() {
        pinMode(adc_pin_num_, INPUT);  // Configure ADC pin as input
        SPI.begin();
    }

    uint16_t read_airSpeed_adc() {
        uint16_t data;
        SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
        digitalWrite(AS_SPI_CS, LOW);  // LOW to enable
        data = SPI.transfer16(0x0000);
        digitalWrite(AS_SPI_CS, HIGH);  // HIGH to disable
        SPI.endTransaction();
        return data;
    }

    float pressure_from_counts(uint16_t counts) {
        float pressure = ((static_cast<float>(counts) - outputmin_) * (pressuremax_ - pressuremin_) / (outputmax_ - outputmin_)) + pressuremin_;
        return pressure;  // pressure (PSI)
    }

private:
    const int adc_pin_num_;  // Specific ADC pin number for this instance

    const float pressuremin_ = 0.0;    // min pressure (PSI)
    const float pressuremax_ = 25.0;   // max pressure (PSI)
    const float outputmin_ = 1638.0;   // min output (counts)
    const float outputmax_ = 14745.0;  // max output (counts)

    // Default SPI pins (shared)
    static const int AS_SPI_CS = 5;
    static const int AS_SPI_MISO = 18;
    static const int AS_SPI_SCLK = 19;
};

#endif // __AIRSPEED_H__
