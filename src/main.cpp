
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Serial.println("TEST");
  Wire.begin();

 
  }


void loop() {
   // Step 1: Tell RTC which register we want (seconds register 0x00)
  Wire.beginTransmission(0x68);
  Wire.write(0x00);               // seconds register
  Wire.endTransmission(false);    // repeated start

  // Step 2: Request 1 byte from RTC
  Wire.requestFrom(0x68, 1);

  // Step 3: Read the returned byte
  if (Wire.available()) {
    uint8_t seconds = Wire.read();
    Serial.print("Raw seconds register: ");
    Serial.println(seconds, HEX);
  Serial.println("Hello World!");
  delay(1000);
  }
}
// #include <SPI.h>
// #include <airSpeed.h>

// #define AS_SPI_CS 5
// #define AS_SPI_MISO 18
// #define AS_SPI_SCLK 19
// #define DUMMY_AS_SPI_MOSI -1
// #define AS_A0 4
// #define AS_A1 16
// #define AS_A2 17

// // Single-ended mode mapping from the first six rows of the datasheet table.
// // Constructor order is (A0, A1, A2).
// AirSpeed_Sensor_Pair A1_B1(0, 0, 0); // A2A1A0 = 000
// AirSpeed_Sensor_Pair A2_B2(1, 0, 0); // A2A1A0 = 001
// AirSpeed_Sensor_Pair A3_B3(0, 1, 0); // A2A1A0 = 010
// AirSpeed_Sensor_Pair A4_B4(1, 1, 0); // A2A1A0 = 011
// AirSpeed_Sensor_Pair A5_B5(0, 0, 1); // A2A1A0 = 100
// AirSpeed_Sensor_Pair A6_B6(1, 0, 1); // A2A1A0 = 101

// AirSpeed_Sensor_Pair airSpeedArray[6] = {A1_B1, A2_B2, A3_B3, A4_B4, A5_B5, A6_B6};
// const uint8_t AIRSPEED_PAIR_COUNT = sizeof(airSpeedArray) / sizeof(airSpeedArray[0]);
// const uint32_t AIRSPEED_PRINT_PERIOD_MS = 1000U;
// uint32_t last_print_ms = 0U;

// void airspeed_init(void);
// void print_airspeed_values(void);
// void print_hex16(uint16_t value);

// void setup() {
//   Serial.begin(9600);
//   while (!Serial) {
//     delay(10);
//   }

//   airspeed_init();

//   Serial.println("time_ms,"
//                  "P1_B_counts,P1_B_psi,P1_A_counts,P1_A_psi,P1_raw0_hex,P1_raw1_hex,"
//                  "P2_B_counts,P2_B_psi,P2_A_counts,P2_A_psi,P2_raw0_hex,P2_raw1_hex,"
//                  "P3_B_counts,P3_B_psi,P3_A_counts,P3_A_psi,P3_raw0_hex,P3_raw1_hex,"
//                  "P4_B_counts,P4_B_psi,P4_A_counts,P4_A_psi,P4_raw0_hex,P4_raw1_hex,"
//                  "P5_B_counts,P5_B_psi,P5_A_counts,P5_A_psi,P5_raw0_hex,P5_raw1_hex,"
//                  "P6_B_counts,P6_B_psi,P6_A_counts,P6_A_psi,P6_raw0_hex,P6_raw1_hex");
// }

// void loop() {
//   const uint32_t now = millis();
//   if ((now - last_print_ms) >= AIRSPEED_PRINT_PERIOD_MS) {
//     last_print_ms = now;
//     print_airspeed_values();
//   }
//   for (int i = 0 ; i < 6 ; i++) {
//     airSpeedArray[i].read_airSpeed_adc_pair();
//   }
//   delay(1);
// }

// void airspeed_init(void) {
//   SPI.begin(AS_SPI_SCLK, AS_SPI_MISO, DUMMY_AS_SPI_MOSI, AS_SPI_CS);

//   pinMode(AS_SPI_CS, OUTPUT);
//   // pinMode(AS_SPI_MISO, INPUT);
//   // pinMode(AS_SPI_SCLK, OUTPUT);
//   digitalWrite(AS_SPI_CS, HIGH);

//   pinMode(AS_A0, OUTPUT);
//   pinMode(AS_A1, OUTPUT);
//   pinMode(AS_A2, OUTPUT);
//   digitalWrite(AS_A0, LOW);
//   digitalWrite(AS_A1, LOW);
//   digitalWrite(AS_A2, LOW);

//   Serial.println("Airspeed Sensor ADC initialized");
// }

// void print_hex16(uint16_t value) {
//   if (value < 0x1000U) Serial.print("0");
//   if (value < 0x0100U) Serial.print("0");
//   if (value < 0x0010U) Serial.print("0");
//   Serial.print(value, HEX);
// }

// void print_airspeed_values(void) {
//   Serial.print("Time: ");
//   Serial.println(millis());

//   for (uint8_t i = 0; i < AIRSPEED_PAIR_COUNT; i++) {
//     AirSpeed_Sensor_Pair& sensor = airSpeedArray[i];
//     sensor.mux_set_writepins();
//     delayMicroseconds(10);

//     AirSpeed_Sensor_Pair::DualADCCounts counts = sensor.read_airSpeed_adc_pair();
//     float psi_b = sensor.pressure_from_counts(counts.adc_b_counts);
//     float psi_a = sensor.pressure_from_counts(counts.adc_a_counts);

//     Serial.print("ADC B Counts: ");
//     Serial.print(counts.adc_b_counts);
//     Serial.print(" ADC B PSI: ");
//     Serial.println(psi_b, 4);

//     Serial.print("ADC A Counts: ");
//     Serial.print(counts.adc_a_counts);

//     Serial.print(", ADC A PSI: ");
//     Serial.println(psi_a, 4);
    
//     // print_hex16(counts.raw_frame0);
//     // Serial.print(",0x");
//     // print_hex16(counts.raw_frame1);
//   }

//   Serial.println();
// }
