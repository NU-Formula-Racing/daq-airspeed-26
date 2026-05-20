/*!
 * @file TCA9548A.h
 *
 * I2C Driver for the TCA9548A 1-to-8 Bidirectional translating switch.
 */

#ifndef _TCA9548A_H
#define _TCA9548A_H

#include "Arduino.h"
#include <Wire.h>

#define TCA9548A_DEFAULT_ADDR 0x70 ///< Default I2C address when A0, A1, A2 are grounded

class TCA9548A {
public:
  TCA9548A();
  
  // Initialization methods
  bool begin(TwoWire *theWire = &Wire);
  bool begin(uint8_t addr, TwoWire *theWire = &Wire);

  // Channel control
  bool selectChannel(uint8_t channel);
  bool closeAllChannels();
  uint8_t getAddress() const;

private:
  uint8_t _addr;
  TwoWire *_wire;
};

#endif // _TCA9548A_H