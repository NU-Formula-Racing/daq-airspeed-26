#include "TCA9548A.h"

TCA9548A::TCA9548A(uint8_t addr, TwoWire &wire)
    : _addr(addr), _wire(wire) {}

bool TCA9548A::begin() {
    _wire.beginTransmission(_addr);
    _wire.write(0x00);  // deselect all channels
    return (_wire.endTransmission() == 0);  // 0 = ACK received
}

void TCA9548A::selectChannel(uint8_t channel) {
    if (channel > 7) return;
    _wire.beginTransmission(_addr);
    _wire.write(1 << channel);
    _wire.endTransmission();
}

void TCA9548A::deselectAll() {
    _wire.beginTransmission(_addr);
    _wire.write(0x00);
    _wire.endTransmission();
}
