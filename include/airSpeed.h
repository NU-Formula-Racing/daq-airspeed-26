#pragma once
#ifndef __AIRSPEED_H__
#define __AIRSPEED_H__

#include <Arduino.h>

enum class AirSpeedPins : uint16_t {
    AS_SPI_CS = 5,      // Airspeed ADC SPI Pins
    AS_A0 = 4,
    AS_A1 = 16,
    AS_A2 = 17,
};

namespace AirSpeedConstants {
    const float PRESSURE_MIN = -1.0;    // min pressure (PSI)
    const float PRESSURE_MAX = 1.0;   // max pressure (PSI)
    const float OUTPUT_MIN = 0.1*4095;   // min output (counts)
    const float OUTPUT_MAX = 0.9*4095;  // max output (counts)
};

// uint16_t number = static_cast<uint16_t>(AirSpeedPins::AS_SPI_CS);

// static_cast<int>(AirSpeedPins::AS_SPI_CS);

class AirSpeed_Sensor_Pair {
    public: 
        AirSpeed_Sensor_Pair(int A0, int A1, int A2) : A0_(A0), A1_(A1), A2_(A2)  {};
        const int A0_;
        const int A1_;
        const int A2_;
        uint16_t read_airSpeed_adc();
        float pressure_from_counts(uint16_t counts);
        void mux_set_writepins();
};


#endif // __AIRSPEED_H__