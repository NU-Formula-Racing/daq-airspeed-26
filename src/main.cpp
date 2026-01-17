#include <Arduino.h>
#include <SPI.h>

#define AS_SPI_CS 5
#define AS_SPI_MISO 18
#define AS_SPI_SCLK 19

float outputmax = 2.97; // output at maximum pressure (Volts)
float outputmin  = 0.33; // output at minimum pressure (Volts)
float pressuremax = 1.0; // maximum value of pressure range (psi)
float pressuremin = -1.0; // minimum value of pressure range (psi)
float outputreading = 0.0; // pressure reading from the sensor

u_int16_t read_airspeed_adc();

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  airspeed_init();
}

void loop() {
  uint16_t as_data = read_airspeed_adc(); // read the data from the Airspeed ADC
  // pressurereading = (((outpu)*())/())+

  delay(1000);
}

// put function definitions here:
void airspeed_init(void) {
  // SPI initialization
  SPI.begin();
  SPI.beginTransaction(SPISettings(2000000, LSBFIRST, SPI_MODE0)); // setting up SPI for the airspeed ADC

  // set up the pins
  pinMode(AS_SPI_CS, OUTPUT); // VSPI CS
  pinMode(AS_SPI_MISO, INPUT); // VSPI MISO
  pinMode(AS_SPI_SCLK, OUTPUT); // VSPI SCLK

  Serial.println("Airspeed Sensors initialized");
}

u_int16_t read_airspeed_adc(void) {
  u_int16_t data;
  digitalWrite(AS_SPI_CS, HIGH); // turn CS high
  data  = SPI.transfer16(0x0000); // receive 16 bits from DoutA&B of the ADC
  digitalWrite(AS_SPI_CS, LOW); // turn CS low
  return data;
}
