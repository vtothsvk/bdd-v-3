#include "Pressure.h"

Pressure::Pressure(AnalogIn *pin){
    this -> sensPin = pin;
}

float Pressure::read(){
    float lastReading = ((3.3*(sensPin -> read()))-0.4407)/0.3157;

    if(lastReading < 0){
        return 0;
    }

    return lastReading;
}