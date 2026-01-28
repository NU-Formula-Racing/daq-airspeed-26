#ifndef __AIRSPEED_H__
#define __AIRSPEED_H__

#include <Arduino.h>
#include <SPI.h>

class AirSpeed_Sensor_Pair {
public: 
    // Constructor to set the specific CS pin
    AirSpeed_Sensor_Pair(int cs_pin) : cs_pin_(cs_pin) {}

    // Initialize pins - call this in setup()
    void init() {
        pinMode(cs_pin_, OUTPUT);
        digitalWrite(cs_pin_, HIGH);  // HIGH to disable
        SPI.begin();
    }

    uint16_t read_airSpeed_adc() {
        uint16_t data;
        SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
        digitalWrite(cs_pin_, LOW);  // LOW to enable
        data = SPI.transfer16(0x0000);
        digitalWrite(cs_pin_, HIGH);  // HIGH to disable
        SPI.endTransaction();
        return data;
    }

    float pressure_from_counts(uint16_t counts) {
        float pressure = ((static_cast<float>(counts) - outputmin_) * (pressuremax_ - pressuremin_) / (outputmax_ - outputmin_)) + pressuremin_;
        return pressure;  // pressure (PSI)
    }

private:
    const int cs_pin_;  // Specific CS pin for this instance

    const float pressuremin_ = 0.0;    // min pressure (PSI)
    const float pressuremax_ = 25.0;   // max pressure (PSI)
    const float outputmin_ = 1638.0;   // min output (counts)
    const float outputmax_ = 14745.0;  // max output (counts)
};

#endif // __AIRSPEED_H__
