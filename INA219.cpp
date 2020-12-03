#include "INA219.h"

INA219::INA219(I2C* boardI2C, uint8_t ADDR){
    this -> myAddr = ADDR << 1;
    this -> moduleI2C = boardI2C;
}

status_t INA219::init(uint16_t calibration, uint8_t resolution){
    if(this -> u16write(INA219_CAL_REG, calibration)){
        return STATUS_TX_ERROR;
    }

    uint16_t res;

    switch(resolution){
        case RES_9b:
            res = INA219_CONFIG_BADCRES_9BIT | INA219_CONFIG_SADCRES_9BIT_1S_84US;
        break;

        case RES_10b:
            res = INA219_CONFIG_BADCRES_10BIT | INA219_CONFIG_SADCRES_10BIT_1S_148US;
        break;

        case RES_11b:
            res = INA219_CONFIG_BADCRES_11BIT | INA219_CONFIG_SADCRES_11BIT_1S_276US;
        break;

        default:
            res = INA219_CONFIG_BADCRES_12BIT | INA219_CONFIG_SADCRES_12BIT_128S_69MS;
        break;
    }

    if(this -> u16write(INA219_CONF_REG, INA219_CONFIG_BVOLTAGERANGE_16V | INA219_CONFIG_GAIN_1_40MV | res | INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS)){
        return STATUS_TX_ERROR;
    }

    return STATUS_OK;
}

status_t INA219::readI(){
    char buffer = INA219_I_REG;

    if(this -> moduleI2C -> write(this -> myAddr, &buffer, 1, false)){
        return STATUS_TX_ERROR;
    }

    if(this -> moduleI2C -> read(this -> myAddr, &(this -> data.rawCurrent_c[0]), 2, false)){
        return STATUS_RX_ERROR;
    }

    this -> data.rawCurrent = (this -> data.rawCurrent_c[0] << 8) | (this -> data.rawCurrent_c[1]);

    #ifdef _DEBUG
    cout << "raw uint16 data: " << this -> data.rawCurrent + 0 << endl;
    #endif

    this -> data.lastCurrent = data.rawCurrent;

    return STATUS_OK;
}

void INA219::setAddr(uint8_t ADDR){
    this -> myAddr = ADDR << 1;
}

float INA219::current(){
    return this -> data.lastCurrent;
}

status_t INA219::u16write(uint8_t reg, uint16_t data){
    char buffer[3];
    buffer[0] = reg;
    buffer[1] = (data >> 8) & 0xff;
    buffer[2] = data & 0xff;

    if(this -> moduleI2C -> write(this -> myAddr, &buffer[0], 3, false)){
        return STATUS_TX_ERROR;
    }

    return STATUS_OK;
}