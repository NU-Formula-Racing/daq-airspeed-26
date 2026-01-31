#include <Arduino.h>
#include <SPI.h>
#include <strainGauge.h>

void Strain_Gauge::init(){
    pinMode(CS_, OUTPUT);
    pinMode(MISO_, OUTPUT);
}

uint16_t Strain_Gauge::read_StrainGauge_data() {
    uint16_t data;
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), LOW);  // LOW to enable
    data = SPI.transfer16(0x0000);
    digitalWrite(static_cast<uint8_t>(AirSpeedPins::AS_SPI_CS), HIGH);  // HIGH to disable
    SPI.endTransaction();
    return data;
}

float Strain_Gauge::strain_from_counts(uint16_t counts) {
    float strain = 0.0; // replace with transfer function when Ryan gives
    return strain;  // strain
}

void Strain_Gauge::file_management() {
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


void sensorCallback() { // read sensors and write to LittleFS
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

// Need data to littleFS, littleFS to SD card
void flushCallback() {
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