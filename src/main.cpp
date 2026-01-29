#include <Arduino.h>
#include <SPI.h>
#include <virtualTimer.h>
#include <airSpeed.h>
#include <FS.h>
#include <LittleFS.h>
#include <SD.h>
// header file (declare functions) another file that calls function 
#define AS_SPI_CS 5      // Airspeed ADC SPI Pins
#define AS_SPI_MISO 18
#define AS_SPI_SCLK 19
#define DUMMY_AS_SPI_MOSI -1
#define AS_A0 4         // Airspeed ADC Multiplexer Pins
#define AS_A1 16
#define AS_A2 17
#define SD_CS 13 // Replace with whatever is correct

// MUX Object declaration
AirSpeed_Sensor_Pair A1_B1(0, 0, 0); // ADC_AS_A1_B1
AirSpeed_Sensor_Pair A2_B2(0, 0, 1); // ADC_AS_A2_B2
AirSpeed_Sensor_Pair A3_B3(0, 1, 0); // ADC_AS_A3_B3
AirSpeed_Sensor_Pair A4_B4(0, 1, 1); // ADC_AS_A4_B4
AirSpeed_Sensor_Pair A5_B5(1, 0, 0); // ADC_AS_A5_B5
AirSpeed_Sensor_Pair A6_B6(1, 0, 1); // ADC_AS_A6_B6


AirSpeed_Sensor_Pair airSpeedArray[6] = {A1_B1, A2_B2, A3_B3, A4_B4, A5_B5, A6_B6};

File lfs_file;  // LittleFS; internal ESP flash storage
File sd_file;   // SD: SD Card storage file

volatile uint8_t mux_state = 0; 
volatile uint8_t adc_state[6] = {0};


void sensorCallback(void);
void flushCallback(void);
void airspeed_init(void);
// cycles multiplexer every 10ms
VirtualTimer sensor_time(100U, sensorCallback, VirtualTimer::Type::kRepeating); // 100ms to LittleFS
VirtualTimer flush_time(5000U, flushCallback, VirtualTimer::Type::kRepeating);



void setup() {
  Serial.begin(9600);
  while (!Serial) {
    delay(10);
  }
  airspeed_init();
  sensor_time.Start(millis());
  flush_time.Start(millis());

  lfs_file = LittleFS.open("/airspeed.csv", FILE_APPEND); // attempts to open file
  if (!lfs_file) { // if file doesn't exist, create it
    lfs_file = LittleFS.open("/airspeed.csv", FILE_WRITE);
    if (lfs_file) {
      lfs_file.println("timestamp_ms,A1_B1,A2_B2,A3_B3,A4_B4,A5_B5,A6_B6");  // Custom headers
      lfs_file.close();
      lfs_file = LittleFS.open("/airspeed.csv", FILE_APPEND); // reopens in FILE_APPEND mode
    }
  }
}

void loop() {
  sensor_time.Tick(millis());
  flush_time.Tick(millis());
  delay(1000);
}

void sensorCallback(void) { // read sensors and write to LittleFS
  uint32_t ts = millis();
  String row; // CSV takes in Strings
  row.reserve(64); // pre-allocate memory
  row += String(ts); // Time Sting for each iteration of 6 sensor readings
  for (int i = 0; i < 6; i++) {
    float psi = airSpeedArray[i].update_reading(); // defined in airSpeed.cpp
    row += ",";
    row += String(psi, 3); // 3 decimal places
  }
  row += "\n";

  if (lfs_file) { // if file is open
    lfs_file.print(row); // writes row to LittleFS file
    lfs_file.flush();  // Commit every N? Or in flushCallback
  }
}

void flushCallback(void) {
  if (!lfs_file) return; // if file doesn't exist

  lfs_file.flush();  // Commit all pending writes

  // Copy LittleFS to SD (APPEND if exists, create if not)
  if (SD.begin(SD_CS)) {
    sd_file = SD.open("/airspeed.csv", FILE_APPEND);
    if (sd_file) {
      File lfs_read = LittleFS.open("/airspeed.csv", FILE_READ);
      if (lfs_read) {
        size_t copied = 0;
        uint8_t buffer[256]; // chunked data buffer (pulls this amount to perform read/write transfer)
        // Copy LittleFS file to SD in chunks to avoid large allocations
        while (lfs_read.available()) {
          size_t read_len = lfs_read.read(buffer, sizeof(buffer)); // read 256-byte chunk
          if (!read_len) {
            break; // Unexpected EOF
          }
          copied += sd_file.write(buffer, read_len); // write chunk to SD
        }
        // The logic behind byte copying is that the same ASCII characters should be transfered if the same bytes are copied
        sd_file.flush();
        sd_file.close();
        lfs_read.close();
      }
    }
  }

  // Clear LittleFS for fresh start
  lfs_file.close();
  LittleFS.remove("/airspeed.csv");  // Delete entire file
  lfs_file = LittleFS.open("/airspeed.csv", FILE_WRITE);  // Recreate empty
  if (lfs_file) {
    lfs_file.println("timestamp_ms,A1_B1,A2_B2,A3_B3,A4_B4,A5_B5,A6_B6");  // New header
    lfs_file.close();
    lfs_file = LittleFS.open("/airspeed.csv", FILE_APPEND);  // Reopen append
  }
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

