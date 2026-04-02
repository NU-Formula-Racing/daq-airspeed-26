#include "airSpeed.h"

// Measurement request command
static const uint8_t kMeasCmd[3] = {0xAA, 0x00, 0x00};

AirSpeedSensor::AirSpeedSensor(TCA9548A &mux, uint8_t channel, TwoWire &wire)
    : _mux(mux), _channel(channel), _wire(wire) {}

bool AirSpeedSensor::begin() {
    _mux.selectChannel(_channel);
    _wire.beginTransmission(ABP2::I2C_ADDR);
    bool VERIFY = (_wire.endTransmission() == 0);
    _mux.deselectAll();
    return VERIFY;
}

ABP2Reading AirSpeedSensor::read() {
    ABP2Reading result{0.0f, 0.0f, 0, false};

    _mux.selectChannel(_channel);

    // 1. Send measurement request
    _wire.beginTransmission(ABP2::I2C_ADDR);
    _wire.write(kMeasCmd, 3);
    if (_wire.endTransmission() != 0) {
        _mux.deselectAll();
        return result;  // I2C error
    }

    // 2. Poll busy bit until ready or timeout
    uint32_t deadline = millis() + ABP2::BUSY_TIMEOUT_MS;
    uint8_t status = 0;
    while (millis() < deadline) {
        if (_wire.requestFrom(ABP2::I2C_ADDR, (uint8_t)1) == 1) {
            status = _wire.read();
            if (!(status & (1 << ABP2::STATUS_BIT_BUSY))) break;
        }
        delay(1);
    }

    // 3. Read all 7 bytes: [status][P2][P1][P0][T2][T1][T0]
    if (_wire.requestFrom(ABP2::I2C_ADDR, (uint8_t)7) != 7) { // checks byte size of request
        _mux.deselectAll();
        return result;  // I2C error
    }

    uint8_t raw[7];
    for (uint8_t i = 0; i < 7; i++) raw[i] = _wire.read();

    _mux.deselectAll();

    result.status = raw[0];

    // Fault check: error or math saturation bits set
    if ((result.status & (1 << ABP2::STATUS_BIT_ERROR)) || (result.status & (1 << ABP2::STATUS_BIT_MATH))) {
        return result;
    }

    // shifts data to form digital signal (from datasheet)
    uint32_t p_counts = ((uint32_t)raw[1] << 16) | ((uint32_t)raw[2] <<  8) | (uint32_t)raw[3];

    uint32_t t_counts = ((uint32_t)raw[4] << 16) | ((uint32_t)raw[5] <<  8) | (uint32_t)raw[6];

    result.pressure_inH2O = _pressure_from_counts(p_counts);
    result.temperature_C  = _temperature_from_counts(t_counts);
    result.valid = true;
    return result;
}

float AirSpeedSensor::_pressure_from_counts(uint32_t counts) {
    return ((float)(counts - ABP2::OUT_MIN) / (float)(ABP2::OUT_MAX - ABP2::OUT_MIN)) * (ABP2::PMAX - ABP2::PMIN) + ABP2::PMIN;
}

float AirSpeedSensor::_temperature_from_counts(uint32_t counts) {
    return ((float)counts * 200.0f / (float)ABP2::TEMP_MAX) - 50.0f;
}
