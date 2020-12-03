#ifndef INA219_h
#define INA219_h

#include "mbed.h"
#include "ina_types.h"
#include <iostream>

class INA219{
public:
    INA219(I2C* boardI2C, uint8_t ADDR);
    status_t init(uint16_t calibration  = DEFAULT_CALIBRATION, uint8_t resolution  = DEFAULT_RESOLUTION);
    status_t readI();
    status_t readV();
    void setAddr(uint8_t ADDR);

    float current();
    float voltage();
    
private:
    ina_data_t data;
    uint8_t myAddr;
    I2C* moduleI2C;

    status_t u16write(uint8_t reg, uint16_t data);
};

#endif