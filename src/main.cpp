#include <Arduino.h>
#include <SPI.h>
#include <virtualTimer.h>

#define AS_SPI_CS 5      // Airspeed ADC SPI Pins
#define AS_SPI_MISO 18
#define AS_SPI_SCLK 19
#define AS_A0 4         // Airspeed ADC Multiplexer Pins
#define AS_A1 16
#define AS_A2 17

// // Multiplexer for the Airspeed ADC
// typedef enum {
//   ADC_AS_A1_B1,
//   ADC_AS_A2_B2,
//   ADC_AS_A3_B3,
//   ADC_AS_A4_B4,
//   ADC_AS_A5_B5,
//   ADC_AS_A6_B6,
// } adc_as_ch;

// static const uint8_t mux_table[][3] = {
//   /*A2 A1 A0*/
//   {0, 0, 0}, // ADC_AS_A1_B1
//   {0, 0, 1}, // ADC_AS_A2_B2
//   {0, 1, 0}, // ADC_AS_A3_B3
//   {0, 1, 1}, // ADC_AS_A4_B4
//   {1, 0, 0}, // ADC_AS_A5_B5
//   {1, 0, 1}, // ADC_AS_A6_B6
// };
volatile uint8_t mux_state = 0; 

void as_multiplexer_cycle(void);

VirtualTimer as_multiplexer_timer(10U, as_multiplexer_cycle, VirtualTimer::Type::kRepeating);


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
  as_multiplexer_timer.Start(millis());
}

void loop() {
  as_multiplexer_timer.Tick(millis());
  uint16_t as_data = read_airspeed_adc(); // read the data from the Airspeed ADC
  // pressurereading = (((outpu)*())/())+

  delay(1000);
}

// put function definitions here:
void airspeed_init(void) {
  // SPI initialization
  SPI.begin(AS_SPI_SCLK,AS_SPI_MISO,,AS_SPI_CS);
  SPI.beginTransaction(SPISettings(2000000, LSBFIRST, SPI_MODE0)); // setting up SPI for the airspeed ADC

  // set up the SPI pins
  pinMode(AS_SPI_CS, OUTPUT); // VSPI CS
  pinMode(AS_SPI_MISO, INPUT); // VSPI MISO
  pinMode(AS_SPI_SCLK, OUTPUT); // VSPI SCLK

  // setting up the multiplexer pins
  pinMode(AS_A0, OUTPUT); // A0
  pinMode(AS_A1, OUTPUT); // A1
  pinMode(AS_A2, OUTPUT); // A2

  Serial.println("Airspeed Sensor ADC initialized");
}

u_int16_t read_airspeed_adc(void) {
  u_int16_t data;
  digitalWrite(AS_SPI_CS, HIGH); // turn CS high
  data  = SPI.transfer16(0x0000); // receive 16 bits from DoutA&B of the ADC
  digitalWrite(AS_SPI_CS, LOW); // turn CS low
  return data;
}

void as_multiplexer_cycle(void){

  GPIO_A0 = (mux_state >> 0) & 0x1;
  GPIO_
}

// void select_adc_as_ch(adc_as_ch ch) {
//   digitalWrite(AS_A2, mux_table[ch] )
// }
