#include <Arduino.h>
#include <SPI.h>
#include <airSpeed.h>

#define AS_SPI_CS 5
#define AS_SPI_MISO 18
#define AS_SPI_SCLK 19
#define DUMMY_AS_SPI_MOSI -1
#define AS_A0 4
#define AS_A1 16
#define AS_A2 17

// Single-ended mode mapping from the first six rows of the datasheet table.
// Constructor order is (A0, A1, A2).
AirSpeed_Sensor_Pair A1_B1(0, 0, 0); // A2A1A0 = 000
AirSpeed_Sensor_Pair A2_B2(1, 0, 0); // A2A1A0 = 001
AirSpeed_Sensor_Pair A3_B3(0, 1, 0); // A2A1A0 = 010
AirSpeed_Sensor_Pair A4_B4(1, 1, 0); // A2A1A0 = 011
AirSpeed_Sensor_Pair A5_B5(0, 0, 1); // A2A1A0 = 100
AirSpeed_Sensor_Pair A6_B6(1, 0, 1); // A2A1A0 = 101

AirSpeed_Sensor_Pair airSpeedArray[6] = {A1_B1, A2_B2, A3_B3, A4_B4, A5_B5, A6_B6};
const uint32_t AIRSPEED_PRINT_PERIOD_MS = 250U;
uint32_t last_print_ms = 0U;

void airspeed_init(void);
void print_airspeed_values(void);

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }

  airspeed_init();

  // Basic CSV output: one row per polling cycle.
  Serial.println("time_ms,B1_counts,B1_psi,B2_counts,B2_psi,B3_counts,B3_psi,B4_counts,B4_psi,B5_counts,B5_psi,B6_counts,B6_psi");
}

void loop() {
  const uint32_t now = millis();
  if ((now - last_print_ms) >= AIRSPEED_PRINT_PERIOD_MS) {
    last_print_ms = now;
    print_airspeed_values();
  }
  delay(1);
}

void airspeed_init(void) {
  SPI.begin(AS_SPI_SCLK, AS_SPI_MISO, DUMMY_AS_SPI_MOSI, AS_SPI_CS);

  pinMode(AS_SPI_CS, OUTPUT);
  pinMode(AS_SPI_MISO, INPUT);
  pinMode(AS_SPI_SCLK, OUTPUT);
  digitalWrite(AS_SPI_CS, HIGH);

  pinMode(AS_A0, OUTPUT);
  pinMode(AS_A1, OUTPUT);
  pinMode(AS_A2, OUTPUT);
  digitalWrite(AS_A0, LOW);
  digitalWrite(AS_A1, LOW);
  digitalWrite(AS_A2, LOW);

  Serial.println("Airspeed Sensor ADC initialized");
}

void print_airspeed_values(void) {
  Serial.print(millis());

  for (int i = 0; i < 6; i++) {
    AirSpeed_Sensor_Pair& sensor = airSpeedArray[i];
    sensor.mux_set_writepins();
    delayMicroseconds(5);

    const AirSpeed_Sensor_Pair::DualADCCounts counts = sensor.read_airSpeed_adc_pair();
    const float psi_b = sensor.pressure_from_counts(counts.adc_b_counts);

    Serial.print(",");
    Serial.print(counts.adc_b_counts);
    Serial.print(",");
    Serial.print(psi_b, 4);
  }

  Serial.println();
}
