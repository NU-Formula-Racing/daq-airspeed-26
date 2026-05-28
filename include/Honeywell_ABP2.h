/*!
 * @file Honeywell_ABP2.h
 *
 * I2C Driver for Honeywell ABP2 Series Digital Pressure Sensors.
 */

#ifndef _HONEYWELL_ABP2_H
#define _HONEYWELL_ABP2_H

#include "Arduino.h"
#include <Wire.h>

// Standard default I2C Address for ABP2 series is 0x28 (unless customized by order)
#define ABP2_I2C_ADDR_DEFAULT 0x28 

class Honeywell_ABP2 {
public:
  Honeywell_ABP2();
  
  bool begin(TwoWire *theWire = &Wire);
  bool begin(uint8_t addr, TwoWire *theWire = &Wire);
  
  // Data collection functions
  bool readSensor();
  
  // Data extraction getters
  float getPressure() const;
  float getTemperature() const;
  uint8_t getStatus() const;

private:
  uint8_t _addr;
  TwoWire *_wire;
  
  uint32_t _rawPressure;
  uint32_t _rawTemperature;
  uint8_t  _status;
  
  float _pressure;
  float _temperature;
  
  void calculatePhysicalValues();
};

#endif // _HONEYWELL_ABP2_H