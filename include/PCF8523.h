#pragma once

#include <Arduino.h>
#include <Wire.h>

namespace PCF8523 {
    constexpr uint8_t I2C_ADDR     = 0x68;

    // Register addresses
    constexpr uint8_t REG_CONTROL1 = 0x00;
    constexpr uint8_t REG_CONTROL2 = 0x01;
    constexpr uint8_t REG_CONTROL3 = 0x02;
    constexpr uint8_t REG_SECONDS  = 0x03;
    constexpr uint8_t REG_MINUTES  = 0x04;
    constexpr uint8_t REG_HOURS    = 0x05;
    constexpr uint8_t REG_DAYS     = 0x06;
    constexpr uint8_t REG_WEEKDAYS = 0x07;
    constexpr uint8_t REG_MONTHS   = 0x08;
    constexpr uint8_t REG_YEARS    = 0x09;

    // Software reset value written to Control_1
    constexpr uint8_t SW_RESET     = 0x58;

    // Control_3: battery switchover standard mode, battery low detection enabled
    constexpr uint8_t CTRL3_BATTERY_STD = 0x00;
}

struct RTCTime {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint8_t year;       // 0-99 (add 2000 for full year)
    bool    os_flag;    // true = clock integrity not guaranteed
    bool    valid;      // false if I2C error
};

class PCF8523_RTC {
public:
    PCF8523_RTC(TwoWire &wire = Wire);

    // Initialize RTC: reset, configure battery backup. Returns false if no ACK.
    bool begin();

    // Read current time from RTC
    RTCTime readTime();

    // Set time on the RTC
    void setTime(uint8_t year, uint8_t month, uint8_t day,
                 uint8_t hour, uint8_t minute, uint8_t second);

private:
    TwoWire &_wire;

    static uint8_t _bcd2bin(uint8_t bcd);
    static uint8_t _bin2bcd(uint8_t bin);
};
