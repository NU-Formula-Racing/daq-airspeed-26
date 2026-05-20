/*!
 * @file TCA9548A.cpp
 *
 * Implementation of the TCA9548A 8-Channel I2C Switch driver.
 */

#include "TCA9548A.h"

TCA9548A::TCA9548A() {
  _addr = TCA9548A_DEFAULT_ADDR;
  _wire = &Wire;
}

bool TCA9548A::begin(uint8_t addr, TwoWire *theWire) {
  _addr = addr;
  _wire = theWire;

  // Simply issue the command to isolate all downstream channels immediately.
  // This cleans up any remnants of prior channel attachments without gating startup.
  closeAllChannels(); 
  delay(10); // Provide a brief physical settling time margin for the bus lines
  
  return true; 
}

bool TCA9548A::begin(TwoWire *theWire) {
  return begin(TCA9548A_DEFAULT_ADDR, theWire);
}

bool TCA9548A::selectChannel(uint8_t channel) {
  if (channel > 7) {
    return false; 
  }

  _wire->beginTransmission(_addr);
  _wire->write(1 << channel); 
  return (_wire->endTransmission() == 0);
}

bool TCA9548A::closeAllChannels() {
  _wire->beginTransmission(_addr);
  _wire->write(0x00); 
  return (_wire->endTransmission() == 0);
}

uint8_t TCA9548A::getAddress() const {
  return _addr;
}