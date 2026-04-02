#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "TCA9548A.h"

// Differential, inH2O, I2C, 3.3V, Transfer Function A
namespace ABP2 {
    constexpr uint8_t  I2C_ADDR   = 0x28;
    constexpr uint32_t OUT_MIN    = 1677722UL;   // 10% of 2^24
    constexpr uint32_t OUT_MAX    = 15099494UL;  // 90% of 2^24
    constexpr float    PMIN       = -2.0f;       // inH2O
    constexpr float    PMAX       =  2.0f;       // inH2O
    constexpr uint32_t TEMP_MAX   = 16777215UL;  // 2^24 - 1
    constexpr uint32_t BUSY_TIMEOUT_MS = 100;

    // Status byte bit positions
    constexpr uint8_t STATUS_BIT_POWER  = 6;
    constexpr uint8_t STATUS_BIT_BUSY   = 5;
    constexpr uint8_t STATUS_BIT_ERROR  = 2;
    constexpr uint8_t STATUS_BIT_MATH   = 0;
}

struct ABP2Reading {
    float   pressure_inH2O;
    float   temperature_C;
    uint8_t status;    // raw status byte
    bool    valid;     // false if timed out, fault, or I2C error
};

class AirSpeedSensor {
public:
    // mux: TCA9548A this sensor connects to; channel: 0–7 on that MUX
    AirSpeedSensor(TCA9548A &mux, uint8_t channel, TwoWire &wire = Wire);

    // Ping the sensor — returns false if no ACK. Call after Wire.begin().
    bool begin();

    // Full measurement cycle: select channel → request → wait → read → deselect
    ABP2Reading read();

private:
    TCA9548A &_mux;
    uint8_t   _channel;
    TwoWire  &_wire;

    static float _pressure_from_counts(uint32_t counts);
    static float _temperature_from_counts(uint32_t counts);
};
