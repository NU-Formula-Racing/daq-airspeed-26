#ifndef __AIRSPEED_H__
#define __AIRSPEED_H__

#include <Arduino.h>

class AirSpeed_Sensor_Pair {
    public: 
        int AirSpeed_Pin(int adc_as_ch);
        uint16_t read_airSpeed_adc(void);
        float getAirSpeed();
        float pressure_from_counts(uint16_t counts);
        
};


#endif // __AIRSPEED_H__