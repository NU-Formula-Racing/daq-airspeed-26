#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <Arduino.h>  // for gpio pins
#include <cstddef>

// debug flags
#define DEBUG  // global debug flag
#define CAN_DEBUG

constexpr size_t AIRSPEED_SENSOR_COUNT = 6U;

enum HWPin {

    // Airspeed ADC (VSPI interface)
    AIRSPEED_SPI_MOSI = GPIO_NUM_23,
    AIRSPEED_SPI_MISO = GPIO_NUM_19,
    AIRSPEED_SPI_SCLK = GPIO_NUM_18,
    AIRSPEED_SPI_CS   = GPIO_NUM_5,

    // Airspeed channel multiplexer select pins (A0..A2)
    AIRSPEED_MUX_SEL0 = GPIO_NUM_4,
    AIRSPEED_MUX_SEL1 = GPIO_NUM_16,
    AIRSPEED_MUX_SEL2 = GPIO_NUM_17,

    // Flowrate Pins
    FLOW_0 = GPIO_NUM_19,
    FLOW_1 = GPIO_NUM_23,

    // IMU Pins
    IMU_INT = GPIO_NUM_13,
    IMU_SDA = GPIO_NUM_21,
    IMU_SCL = GPIO_NUM_22,

    // Active Aero Pins
    ACTIVE_AERO_0 = GPIO_NUM_25,
    ACTIVE_AERO_1 = GPIO_NUM_26,

    // Temperature Sensor Pins
    TEMP_0 = GPIO_NUM_12,
    TEMP_1 = GPIO_NUM_15,

    // CAN Pins
    DATA_CAN_RX = GPIO_NUM_4,
    STATUS_DATA = GPIO_NUM_16,
    STATUS_CLK = GPIO_NUM_17,
    DATA_CAN_TX = GPIO_NUM_5,

    // General Pins (don't use directly)
    ESP_RX = GPIO_NUM_3,
    ESP_TX = GPIO_NUM_1,

};

#endif  // __DEFINE_H__