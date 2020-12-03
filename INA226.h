#ifndef INA226_h
#define INA226_h

#include "mbed.h"
#include "INA226_types.h"
#include <iostream>

class INA226{
public:
    INA226(I2C* boardI2C, uint8_t ADDR);
    status_t reset();
    status_t init(uint16_t calibration = DEFAULT_CALIBRATION, uint8_t resolution = DEFAULT_RESOLUTION, uint16_t averaging = DEFAULT_AVERAGED_S, uint8_t mode = INA266_DEFAULT_MODE);
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