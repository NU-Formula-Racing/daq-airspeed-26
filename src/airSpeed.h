#pragma once
#ifndef __AIRSPEED_H__
#define __AIRSPEED_H__

#include <Arduino.h>

enum class AirSpeedPins : uint16_t {
    AS_SPI_CS = 5,      // Airspeed ADC SPI Pins
    AS_SPI_MISO = 18,
    AS_SPI_SCLK = 19,
};

namespace AirSpeedConstants {
    const float PRESSURE_MIN = 0.0;    // min pressure (PSI)
    const float PRESSURE_MAX = 25.0;   // max pressure (PSI)
    const float OUTPUT_MIN = 1638.0;   // min output (counts)
    const float OUTPUT_MAX = 14745.0;  // max output (counts)
};

// uint16_t number = static_cast<uint16_t>(AirSpeedPins::AS_SPI_CS);

// static_cast<int>(AirSpeedPins::AS_SPI_CS);

class AirSpeed_Sensor_Pair {
    public: 
        AirSpeed_Sensor_Pair(int adc_pin_num) : adc_pin_num_(adc_pin_num) {};
        void init();
        uint16_t read_airSpeed_adc();
        float pressure_from_counts(uint16_t counts);
        const int adc_pin_num_;  // Specific ADC pin number for this instance
        
};


#endif // __AIRSPEED_H__