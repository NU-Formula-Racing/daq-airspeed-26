#include "PCF8523.h"

PCF8523_RTC::PCF8523_RTC(TwoWire &wire) : _wire(wire) {}

bool PCF8523_RTC::begin() {
    // Check if RTC is present on the bus
    _wire.beginTransmission(PCF8523::I2C_ADDR);
    if (_wire.endTransmission() != 0) return false;

    // Software reset
    _wire.beginTransmission(PCF8523::I2C_ADDR);
    _wire.write(PCF8523::REG_CONTROL1);
    _wire.write(PCF8523::SW_RESET);
    _wire.endTransmission();

    delay(2);  // allow reset to complete

    // Configure Control_3: standard battery switchover, low-detection enabled
    _wire.beginTransmission(PCF8523::I2C_ADDR);
    _wire.write(PCF8523::REG_CONTROL3);
    _wire.write(PCF8523::CTRL3_BATTERY_STD);
    _wire.endTransmission();

    return true;
}

RTCTime PCF8523_RTC::readTime() {
    RTCTime t{0, 0, 0, 0, 0, 0, 0, false, false};

    // Point to seconds register, then read 7 consecutive bytes
    _wire.beginTransmission(PCF8523::I2C_ADDR);
    _wire.write(PCF8523::REG_SECONDS);
    if (_wire.endTransmission(false) != 0) return t;  // repeated start

    if (_wire.requestFrom(PCF8523::I2C_ADDR, (uint8_t)7) != 7) return t;

    uint8_t raw[7];
    for (uint8_t i = 0; i < 7; i++) raw[i] = _wire.read();

    // OS flag is bit 7 of seconds register
    t.os_flag = (raw[0] & 0x80) != 0;

    t.seconds = _bcd2bin(raw[0] & 0x7F);
    t.minutes = _bcd2bin(raw[1] & 0x7F);
    t.hours   = _bcd2bin(raw[2] & 0x3F);  // 24-hour mode, bits 5:0
    t.day     = _bcd2bin(raw[3] & 0x3F);
    t.weekday = raw[4] & 0x07;
    t.month   = _bcd2bin(raw[5] & 0x1F);
    t.year    = _bcd2bin(raw[6]);
    t.valid   = true;

    return t;
}

void PCF8523_RTC::setTime(uint8_t year, uint8_t month, uint8_t day,
                           uint8_t hour, uint8_t minute, uint8_t second) {
    _wire.beginTransmission(PCF8523::I2C_ADDR);
    _wire.write(PCF8523::REG_SECONDS);
    _wire.write(_bin2bcd(second));
    _wire.write(_bin2bcd(minute));
    _wire.write(_bin2bcd(hour));
    _wire.write(_bin2bcd(day));
    _wire.write(0);  // weekday — set to 0 (Sunday)
    _wire.write(_bin2bcd(month));
    _wire.write(_bin2bcd(year));
    _wire.endTransmission();

    // Clear OS flag by reading then writing seconds register with bit 7 = 0
    _wire.beginTransmission(PCF8523::I2C_ADDR);
    _wire.write(PCF8523::REG_SECONDS);
    if (_wire.endTransmission(false) == 0) {
        if (_wire.requestFrom(PCF8523::I2C_ADDR, (uint8_t)1) == 1) {
            uint8_t sec = _wire.read();
            _wire.beginTransmission(PCF8523::I2C_ADDR);
            _wire.write(PCF8523::REG_SECONDS);
            _wire.write(sec & 0x7F);  // clear OS flag
            _wire.endTransmission();
        }
    }
}

uint8_t PCF8523_RTC::_bcd2bin(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

uint8_t PCF8523_RTC::_bin2bcd(uint8_t bin) {
    return ((bin / 10) << 4) | (bin % 10);
}
