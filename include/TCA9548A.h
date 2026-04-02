#ifndef TCA9548A_H
#define TCA9548A_H

#include <Arduino.h>
#include <Wire.h>

class TCA9548A {
public:
    // addr: I2C address of the MUX (0x70–0x77 based on A0/A1/A2 pins)
    TCA9548A(uint8_t addr, TwoWire &wire = Wire);

    // Call once in setup() — returns false if no ACK received from the MUX
    bool begin();

    // Enable a single channel (0–7); disables all others
    void selectChannel(uint8_t channel);

    // Disable all channels
    void deselectAll();

private:
    uint8_t _addr;
    TwoWire &_wire;
};

#endif
