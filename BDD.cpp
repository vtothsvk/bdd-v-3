#include "BDD.h"
#include "mbed.h"
#include <cstdlib>
#include "drivers/I2C.h"

BDD::BDD(I2C *boardI2C, uint8_t ADDR){
    moduleI2C = boardI2C;
    this -> moduleAddress = ADDR << 1;
    ch[0].reg = 0x01;
    ch[1].reg = 0x03;
}

status_t BDD::setTime(bool channel, bool polarity, long time){
    char message[5];
    message[0] = (this -> ch[channel].reg) + polarity;
    bdd_transfer_buffer_t buffer;
    buffer.data = (unsigned long)time;

    for(uint8_t i = 0; i <= 3; i++){
        message[1 + i] = buffer.dataBuff[3 - i];
    }    

    if(this -> moduleI2C -> write(this -> moduleAddress, &message[0], 5, false)){
        return STATUS_TX_ERROR;
    }

    wait_ms(10);
    return STATUS_OK;
}

status_t BDD::setDuty(bool channel, uint8_t duty_cycle){
    char message[2];
    message[0] = (this -> ch[channel].reg) + 8;
    message[1] = duty_cycle;

    if(this -> moduleI2C -> write(this -> moduleAddress, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    wait_ms(10);
    return STATUS_OK;
}

status_t BDD::getTime(bool channel, bool polarity){
    char message = (this -> ch[channel].reg) + polarity + 4;
    bdd_transfer_buffer_t buffer;

    if(this -> moduleI2C -> write(this -> moduleAddress, &message, 1, false)){
        return STATUS_TX_ERROR;
    }
    wait_ms(10);
    //if no go try 4 byte to r_buffer then assign dataBuff = r_buffer<-
        if(this -> moduleI2C -> read(this -> moduleAddress, buffer.dataBuff, 4, false)){
            return STATUS_RX_ERROR;
        }

    this -> ch[channel].time[polarity] = *(unsigned long*)&(buffer.dataBuff);
    return STATUS_OK;
}

status_t BDD::getDuty(bool channel){
    char message = (this -> ch[channel].reg) + 9;
    char r_buffer;

    if(this -> moduleI2C -> write(this -> moduleAddress, &message, 1, false)){
        return STATUS_TX_ERROR;
    }
    wait_ms(10);
    if(this -> moduleI2C -> read(this -> moduleAddress, &r_buffer, 1, false)){
        return STATUS_RX_ERROR;
    }
    this -> ch[channel].dc = *(uint8_t*)&r_buffer + 0;
    return STATUS_OK;
}

status_t BDD::getPol(bool channel){
    char message = (this -> ch[channel].reg) + 12;
    char r_buffer;
    
    if(this -> moduleI2C -> write(this -> moduleAddress, &message, 1, false)){
        return STATUS_TX_ERROR;
    }
    wait_ms(10);
    if(this -> moduleI2C -> read(this -> moduleAddress, &r_buffer, 1, false)){
        return STATUS_RX_ERROR;
    }
    this -> ch[channel].pol = *(uint8_t*)&(r_buffer) + 0;
    return STATUS_OK;
}

status_t BDD::pacInit(bool channel, float shunt, uint8_t ADDR){
    ch[channel].pacAddr = ADDR << 1;
    ch[channel].shunt = shunt;
    char message[2];
    
    message[0] = pacConfigReg;
    message[1] = pacConfiguration;

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    wait_ms(100);
    message[0] = pacConverionReg;
    message[1] = pacConversion;

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    wait_ms(100);
    message[0] = pacSourceSamplingReg;
    message[1] = pacSourceSampling;

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    wait_ms(100);
    message[0] = pacSenseSamplingReg;
    message[1] = pacSenseSampling;

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &message[0], 2, false)){
        return STATUS_TX_ERROR;
    }

    wait_ms(10);
    return STATUS_OK;
}

status_t BDD::getI(bool channel){
        char buffer[2];
        int Vsense;
        buffer[0] = sensReg;
        buffer[1] = sensReg + 1;

        if(this -> moduleI2C -> write(ch[channel].pacAddr, &buffer[0], 1, false)){
            return STATUS_TX_ERROR;
        }

        if(this -> moduleI2C -> read(ch[channel].pacAddr, &buffer[0], 1, false)){
            return STATUS_RX_ERROR;
        }

        if(this -> moduleI2C -> write(ch[channel].pacAddr, &buffer[1], 1, false)){
            return STATUS_TX_ERROR;
        }

        if(this -> moduleI2C -> read(ch[channel].pacAddr, &buffer[1], 1, false)){
            return STATUS_RX_ERROR;
        }

        Vsense = buffer[1] | (buffer[0] << 8);
        Vsense = Vsense >> 4;
        ch[channel].i = (0.08 / ch[channel].shunt) * (Vsense / 2047.0);
        wait_ms(10);
        return STATUS_OK;
}

status_t BDD::getV(bool channel){
    char buffer[2];
    float v;
    int Vsource;
    buffer[0] = sourceReg;
    buffer[1] = sourceReg + 1;

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &buffer[0], 1, false)){
        return STATUS_TX_ERROR;
    }

    if(this -> moduleI2C -> read(ch[channel].pacAddr, &buffer[0], 1, false)){
        return STATUS_RX_ERROR;
    }

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &buffer[1], 1, false)){
        return STATUS_TX_ERROR;
    }

    if(this -> moduleI2C -> read(ch[channel].pacAddr, &buffer[1], 1, false)){
        return STATUS_RX_ERROR;
    }

    Vsource = buffer[1] | (buffer[0] << 8);
    Vsource = Vsource >> 5;
    v = (40.0 - (40.0 / 2048.0)) * (Vsource / 2047.0);  //meni sa hodnot Rshunt
    if (v < 0) {
        v = v + 40.0;
    }
    ch[channel].v = v;
    wait_ms(10);
    return STATUS_OK;
}

status_t BDD::getP(bool channel){
    char buffer[2];
    int P;
    buffer[0] = powerReg;
    buffer[1] = powerReg + 1;

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &buffer[0], 1, false)) {
        return STATUS_TX_ERROR;
    }

    if(this -> moduleI2C -> read(ch[channel].pacAddr, &buffer[0], 1, false)){
        return STATUS_RX_ERROR;
    }

    if(this -> moduleI2C -> write(ch[channel].pacAddr, &buffer[1], 1, false)){
        return STATUS_TX_ERROR;
    }

    if(this -> moduleI2C -> read(ch[channel].pacAddr, &buffer[1], 1, false)){
        return STATUS_RX_ERROR;
    }

   P = buffer[1] | (buffer[0] << 8);
  //Vsource = Vsource >> 5;
  ch[channel].p = ((40.0 - (40.0 / 2048.0)) * (0.08 / ch[channel].shunt)) * (P / 65535.0);
  return STATUS_OK;
}

float BDD::voltage(bool channel){
    return this -> ch[channel].v;
}

float BDD::current(bool channel){
    return this -> ch[channel].i;
}

float BDD::power(bool channel){
    return this -> ch[channel].p;
}

long BDD::time(bool channel, bool polarity){
    return this -> ch[channel].time[polarity];
}

uint8_t BDD::duty(bool channel){
    return this -> ch[channel].dc;
}

bool BDD::polarity(bool channel){
    return this -> ch[channel].pol;
}