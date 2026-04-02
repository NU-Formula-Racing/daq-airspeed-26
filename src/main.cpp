#include <Arduino.h>
#include <Wire.h>
#include <virtualTimer.h>
#include "TCA9548A.h"
#include "airSpeed.h"
#include "PCF8523.h"

// GPIO Definitions
#define SDA_PIN 21
#define SCL_PIN 22

// SD card pins
#define SD_MISO 27
#define SD_CLK  14
#define SD_MOSI 13
#define SD_CS   15

// Hardware stuff
TCA9548A muxA(0x70);          // MUX A at address 0x70
TCA9548A muxB(0x71);          // MUX B at address 0x71
PCF8523_RTC rtc;

// Airspeed sensors
// MUX A channels 0-7, MUX B channels 0-3  (adjust to match your wiring)
static constexpr uint8_t MUX_A_COUNT = 6;
static constexpr uint8_t MUX_B_COUNT = 6;
static constexpr uint8_t SENSOR_COUNT = MUX_A_COUNT + MUX_B_COUNT;

AirSpeedSensor sensorsA[] = {
    AirSpeedSensor(muxA, 0), AirSpeedSensor(muxA, 1),
    AirSpeedSensor(muxA, 2), AirSpeedSensor(muxA, 3),
    AirSpeedSensor(muxA, 4), AirSpeedSensor(muxA, 5),
};

AirSpeedSensor sensorsB[] = {
    AirSpeedSensor(muxB, 0), AirSpeedSensor(muxB, 1),
    AirSpeedSensor(muxB, 2), AirSpeedSensor(muxB, 3),
    AirSpeedSensor(muxB, 4), AirSpeedSensor(muxB, 5),
};

// Readings storage
ABP2Reading readings[SENSOR_COUNT];
RTCTime currentTime;

// Timer group
VirtualTimerGroup timerGroup;

// Forward declarations
void pollSensors();
void printData();

// Callbacks
void pollSensors() {
    // Read RTC
    currentTime = rtc.readTime();

    // Read all MUX A sensors
    for (uint8_t i = 0; i < MUX_A_COUNT; i++) {
        readings[i] = sensorsA[i].read();
    }

    // Read all MUX B sensors
    for (uint8_t i = 0; i < MUX_B_COUNT; i++) {
        readings[MUX_A_COUNT + i] = sensorsB[i].read();
    }
}

void printData() {
    // Print RTC timestamp
    if (currentTime.valid) {
        char ts[24];
        snprintf(ts, sizeof(ts), "20%02u-%02u-%02u %02u:%02u:%02u",
                 currentTime.year, currentTime.month, currentTime.day,
                 currentTime.hours, currentTime.minutes, currentTime.seconds);
        Serial.print("[");
        Serial.print(ts);
        Serial.print("] ");
        if (currentTime.os_flag) Serial.print("(RTC CLOCK LOST) ");
    } else {
        Serial.print("[RTC ERROR] ");
    }

    Serial.print("t=");
    Serial.print(millis());
    Serial.println("ms");

    // Print each sensor reading
    for (uint8_t i = 0; i < SENSOR_COUNT; i++) {
        const char *muxLabel = (i < MUX_A_COUNT) ? "A" : "B";
        uint8_t ch = (i < MUX_A_COUNT) ? i : (i - MUX_A_COUNT);

        Serial.print("  MUX");
        Serial.print(muxLabel);
        Serial.print(":CH");
        Serial.print(ch);

        if (readings[i].valid) {
            Serial.print("  P=");
            Serial.print(readings[i].pressure_inH2O, 4);
            Serial.print(" inH2O  T=");
            Serial.print(readings[i].temperature_C, 1);
            Serial.println(" C");
        } else {
            Serial.print("  INVALID (status=0x"); // error catching
            Serial.print(readings[i].status, HEX);
            Serial.println(")");
        }
    }
    Serial.println("---");
}

void setup() {
    Serial.begin(9600);
    Wire.begin(SDA_PIN, SCL_PIN);

    Serial.println("DAQ Airspeed — Initializing...");

    // Init MUXes
    if (!muxA.begin()) Serial.println("ERROR: MUX A (0x70) not found!");
    if (!muxB.begin()) Serial.println("ERROR: MUX B (0x71) not found!");

    // Init RTC
    if (!rtc.begin()) {
        Serial.println("ERROR: PCF8523 RTC not found!");
    } else {
        Serial.println("PCF8523 RTC OK");
    }

    // Init sensors — report which ones respond
    Serial.print("Sensors on MUX A: ");
    for (uint8_t i = 0; i < MUX_A_COUNT; i++) {
        bool ok = sensorsA[i].begin();
        Serial.print(ok ? "+" : "-");
    }
    Serial.println();

    Serial.print("Sensors on MUX B: ");
    for (uint8_t i = 0; i < MUX_B_COUNT; i++) {
        bool ok = sensorsB[i].begin();
        Serial.print(ok ? "+" : "-");
    }
    Serial.println();

    // CSV-style header for serial
    Serial.println();
    Serial.println("timestamp,millis,mux,ch,pressure_inH2O,temp_C,status,valid");
    Serial.println("========================================================");

    // Schedule repeating timers
    timerGroup.AddTimer(1000, pollSensors);   // poll sensors every 1s
    timerGroup.AddTimer(1000, printData);     // print to serial every 1s

    Serial.println("Timers started.");
}

void loop() {
    timerGroup.Tick(millis());
}
