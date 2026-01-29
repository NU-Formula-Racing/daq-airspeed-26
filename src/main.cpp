#include <Arduino.h>
#include <SPI.h>
#include <virtualTimer.h>
#include <airSpeed.h>
// header file (declare functions) another file that calls function 
#define AS_SPI_CS 5      // Airspeed ADC SPI Pins
#define AS_SPI_MISO 18
#define AS_SPI_SCLK 19
#define DUMMY_AS_SPI_MOSI -1
#define AS_A0 4         // Airspeed ADC Multiplexer Pins
#define AS_A1 16
#define AS_A2 17

// MUX Object declaration
AirSpeed_Sensor_Pair A1_B1(0, 0, 0); // ADC_AS_A1_B1
AirSpeed_Sensor_Pair A2_B2(0, 0, 1); // ADC_AS_A2_B2
AirSpeed_Sensor_Pair A3_B3(0, 1, 0); // ADC_AS_A3_B3
AirSpeed_Sensor_Pair A4_B4(0, 1, 1); // ADC_AS_A4_B4
AirSpeed_Sensor_Pair A5_B5(1, 0, 0); // ADC_AS_A5_B5
AirSpeed_Sensor_Pair A6_B6(1, 0, 1); // ADC_AS_A6_B6


volatile uint8_t mux_state = 0; 
volatile uint8_t adc_state[6] = {0};


void adc_cycle(void);
// cycles multiplexer every 10ms
VirtualTimer adc_cycle_timer(100U, adc_cycle, VirtualTimer::Type::kRepeating);


void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  airspeed_init();
  adc_cycle_timer.Start(millis());
}

void loop() {
  adc_cycle_timer.Tick(millis());
  uint16_t as_data = read_airspeed_adc(); // read the data from the Airspeed ADC

  float pressure = pressure_from_counts(as_data); // convert counts to pressure
  delay(1000);
}

// put function definitions here:
void airspeed_init(void) {
  // SPI initialization
  SPI.begin(AS_SPI_SCLK, AS_SPI_MISO, DUMMY_AS_SPI_MOSI, AS_SPI_CS);  // Need MOSI?
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

enum ADCState {
  STATE_SWITCH_CHANNEL,
  STATE_READ_ADC
};

volatile uint16_t adc_readings[6] = {0};

void adc_cycle(void) {
  if (adc_state == STATE_SWITCH_CHANNEL) {
    select_adc_as_ch(static_cast<adc_as_ch>(mux_state)); // switch to next MUX state
    mux_state = (mux_state + 1) % 6; // make 6 as a define var
    adc_state = STATE_READ_ADC;
    
    adc_timer.Interval(40U); // 40ms settle time
  } 
  else if (adc_state == STATE_READ_ADC) {
    uint8_t channel = (mux_state == 0) ? 5 : (mux_state - 1); // reads ADC
    adc_readings[channel] = read_airspeed_adc();
    adc_state = STATE_SWITCH_CHANNEL;
    
    adc_timer.Interval(60U); // completes 100ms cycle
  }
}

