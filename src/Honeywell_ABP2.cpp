/*!
 * @file Honeywell_ABP2.cpp
 *
 * Driver implementation for Honeywell ABP2 Digital 24-bit Pressure Sensors.
 */

#include "Honeywell_ABP2.h"

Honeywell_ABP2::Honeywell_ABP2() {
  _addr = ABP2_I2C_ADDR_DEFAULT;
  _wire = &Wire;
  _pressure = 0.0;
  _temperature = 0.0;
  _status = 0;
}

/**
 * @brief Setups the HW by sending a dummy measurement request to wake up the sensor.
 */
bool Honeywell_ABP2::begin(uint8_t addr, TwoWire *theWire) {
  _addr = addr;
  _wire = theWire;
  
  // WAKE UP THE SENSOR: Send an initial measurement command sequence.
  // This awakens the internal ASIC state machine from deep sleep mode.
  _wire->beginTransmission(_addr);
  _wire->write(0xAA); 
  _wire->write(0x00); 
  _wire->write(0x00); 
  
  // If the sensor is physically wired and powered correctly, it will ACK this command block.
  if (_wire->endTransmission() == 0) {
    delay(5); // Wait for the wake conversion to complete cleanly
    return true;
  }
  return false;
}

bool Honeywell_ABP2::begin(TwoWire *theWire) {
  return begin(ABP2_I2C_ADDR_DEFAULT, theWire);
}

/**
 * @brief Requests and captures a fresh pressure and temperature reading packet.
 */
bool Honeywell_ABP2::readSensor() {
  // Step 1: Send the Start Measurement Request Command 
  _wire->beginTransmission(_addr);
  _wire->write(0xAA); // Measurement Command byte
  _wire->write(0x00); // Parameter 1
  _wire->write(0x00); // Parameter 2
  if (_wire->endTransmission() != 0) {
    return false; // Communication failure
  }
  
  // Step 2: Wait for Conversion to finish
  delay(5); 
  
  // Step 3: Request the complete 7-byte output block 
  uint8_t bytesReceived = _wire->requestFrom(_addr, (uint8_t)7);
  if (bytesReceived < 7) {
    return false; // Missing data bytes
  }
  
  // Read Status Byte
  _status = _wire->read();
  
  // Read 24-bit raw Pressure payload
  _rawPressure = (uint32_t)_wire->read() << 16;
  _rawPressure |= (uint32_t)_wire->read() << 8;
  _rawPressure |= (uint32_t)_wire->read();
  
  // Read 24-bit raw Temperature payload
  _rawTemperature = (uint32_t)_wire->read() << 16;
  _rawTemperature |= (uint32_t)_wire->read() << 8;
  _rawTemperature |= (uint32_t)_wire->read();
  
  // Step 4: Run transfer calculations
  calculatePhysicalValues();
  return true;
}

/**
 * @brief Converts raw counts to standard physical scale values.
 */
void Honeywell_ABP2::calculatePhysicalValues() {
  float outMin = 1677722.0;  // 10% of 2^24 digital counts
  float outMax = 15099494.0; // 90% of 2^24 digital counts
  
  // TODO: Check your exact sensor part number and adjust these calibration boundaries!
  // e.g., if you have a 0 to 4 bar sensor, set pMax = 4.0
  float pMin = 0.0;  
  float pMax = 10.0; 

  _pressure = (((float)_rawPressure - outMin) * (pMax - pMin)) / (outMax - outMin) + pMin;
  _temperature = (((float)_rawTemperature / 16777216.0) * 150.0) - 40.0;
}

float Honeywell_ABP2::getPressure() const { return _pressure; }
float Honeywell_ABP2::getTemperature() const { return _temperature; }
uint8_t Honeywell_ABP2::getStatus() const { return _status; }