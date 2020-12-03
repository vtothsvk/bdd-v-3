#ifndef Pressure_h
#define Pressure_h

#include "mbed.h"

class Pressure{
public:
    Pressure(AnalogIn *pin);
    float read();
private:
    AnalogIn *sensPin;
};

#endif