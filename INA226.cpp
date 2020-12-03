#include "INA226.h"

#define _DEBUG

INA226::INA226(I2C* boardI2C, uint8_t ADDR){
    this -> moduleI2C = boardI2C;
    this -> myAddr = ADDR << 1;
}//INA226

status_t INA226::reset(){
    if(this -> u16write(INA226_CONF_REG, INA226_CONFIG_RESET)){
        return STATUS_TX_ERROR;
    }

    return STATUS_OK;
}//INA226::reset

status_t INA226::init(uint16_t calibration, uint8_t resolution, uint16_t averaging, uint8_t mode){
    if(this -> u16write(INA226_CAL_REG, calibration)){
        return STATUS_TX_ERROR;
    }

    uint16_t config = averaging | mode;

    switch(resolution){
        case RES_9b:
            config |= (INA226_CONFIG_VBUS_140 | INA226_CONFIG_SHUNT_140);
        break;
        
        case RES_10b:
            config |= (INA226_CONFIG_VBUS_204 | INA226_CONFIG_SHUNT_204);
        break;

        case RES_11b:
            config |= (INA226_CONFIG_VBUS_332 | INA226_CONFIG_SHUNT_332);
        break;

        case RES_12b:
            config |= (INA226_CONFIG_VBUS_588 | INA226_CONFIG_SHUNT_588);
        break;

        case RES_13b:
            config |= (INA226_CONFIG_VBUS_1100 | INA226_CONFIG_SHUNT_1100);
        break;

        case RES_14b:
            config |= (INA226_CONFIG_VBUS_2116 | INA226_CONFIG_SHUNT_2116);
        break;

        case RES_15b:
            config |= (INA226_CONFIG_VBUS_4156 | INA226_CONFIG_SHUNT_4156);
        break;

        case RES_16b:
            config |= (INA226_CONFIG_VBUS_8244 | INA226_CONFIG_SHUNT_8244);
        break;
    }//switch

    if(this -> u16write(INA226_CONF_REG, config)){
        return STATUS_TX_ERROR;
    }
    
    return STATUS_OK;
}//INA226::init

status_t INA226::readI(){
    char buffer = INA226_I_REG;

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
    if(this -> data.rawCurrent > 65000){
        this -> data.lastCurrent = 0;    
    }else{
        this -> data.lastCurrent = this -> data.rawCurrent;
    }

    return STATUS_OK;
}//INA226::read

void INA226::setAddr(uint8_t ADDR){
    this -> myAddr = ADDR << 1;
}//INA226::setAddr

float INA226::current(){
    return this -> data.lastCurrent;
}

status_t INA226::u16write(uint8_t reg, uint16_t data){
    char buffer[3];
    buffer[0] = reg;
    buffer[1] = (data >> 8) & 0xff;
    buffer[2] = data & 0xff;

    if(this -> moduleI2C -> write(this -> myAddr, &buffer[0], 3, false)){
        return STATUS_TX_ERROR;
    }

    return STATUS_OK;
}