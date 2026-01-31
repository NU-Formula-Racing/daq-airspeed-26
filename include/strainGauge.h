#pragma once
#ifndef __STRAINGAUGE_H__
#define __STRAINGAUGE_H__

#include <Arduino.h>

enum class StrainGaugePins : uint16_t {
    SG_ADC_CLK = 5,      // Strain Gauge ADC Clock Pin (change to correct)
};

class Strain_Gauge {
    public: 
        Strain_Gauge(int CS, int MISO) : CS_(CS), MISO_(MISO), {};
        const int CS_;
        const int MISO_;
        void init();         // SPI initiation
        uint16_t read_StrainGauge_data();
        float strain_from_counts(uint16_t counts);
        void file_management(); // file management
};

// Six different LS/SD card files because strain gauge will be on different parts of the car so each reading should be treated as separate data packets
#endif // __STRAINGAUGE_H__