#include <Wire.h>
#include "TCA9548A.h"       // Use your multiplexer driver architecture
#include "Honeywell_ABP2.h" // Use the new ABP2 driver template

#define MUX1_ADDRESS 0x70   
#define MUX2_ADDRESS 0x71   

TCA9548A mux1;
TCA9548A mux2;

// Arrays representing the actual Honeywell sensor items connected to each MUX board
Honeywell_ABP2 sensorsMux1[6]; 
Honeywell_ABP2 sensorsMux2[6]; 

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  Wire.begin();
  delay(100);

  // 1. Initialize Multiplexer 1
  if (!mux1.begin(MUX1_ADDRESS, &Wire)) {
    Serial.println("CRITICAL ERROR: Multiplexer 1 (0x70) not responding!");
    while(1); 
  }
  Serial.println("Multiplexer 1 (0x70) initialized successfully.");

  delay(100); // <--- ADD THIS CRITICAL DELAY HERE to let the bus settle!

  // 2. Initialize Multiplexer 2
  if (!mux2.begin(MUX2_ADDRESS, &Wire)) {
    Serial.println("CRITICAL ERROR: Multiplexer 2 (0x71) not responding!");
    while(1); 
  }
  Serial.println("Multiplexer 2 (0x71) initialized successfully.");
  
  delay(100);

  // Initialize ABP2 devices under Multiplexer 1
  for (uint8_t i = 0; i < 6; i++) {
    mux1.selectChannel(i);
    delay(5); 

    if (!sensorsMux1[i].begin(ABP2_I2C_ADDR_DEFAULT, &Wire)) {
      Serial.print("Sensor not found on MUX 1 [Ch "); Serial.print(i); Serial.println("]");
    }
  }

  // Initialize ABP2 devices under Multiplexer 2
  for (uint8_t i = 0; i < 6; i++) {
    mux2.selectChannel(i);
    delay(5); 

    if (!sensorsMux2[i].begin(ABP2_I2C_ADDR_DEFAULT, &Wire)) {
      Serial.print("Sensor not found on MUX 2 [Ch "); Serial.print(i); Serial.println("]");
    }
  }
}

void loop() {
  // Read sensors from Multiplexer 1
  for (uint8_t i = 0; i < 6; i++) {
    mux1.selectChannel(i);
    
    if (sensorsMux1[i].readSensor()) {
      Serial.print("MUX 1 [Ch "); Serial.print(i);
      Serial.print("] Pressure: "); Serial.print(sensorsMux1[i].getPressure(), 4);
      Serial.print(" | Temp: "); Serial.println(sensorsMux1[i].getTemperature(), 1);
    }
  }

  // Read sensors from Multiplexer 2
  for (uint8_t i = 0; i < 6; i++) {
    mux2.selectChannel(i);
    
    if (sensorsMux2[i].readSensor()) {
      Serial.print("MUX 2 [Ch "); Serial.print(i);
      Serial.print("] Pressure: "); Serial.print(sensorsMux2[i].getPressure(), 4);
      Serial.print(" | Temp: "); Serial.println(sensorsMux2[i].getTemperature(), 1);
    }
  }

  Serial.println("==========================================");
  delay(1000); 
}