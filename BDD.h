#ifndef BDD_h
#define BDD_h

#include "mbed.h"
#include "BDD_types.h"
#include "drivers/I2C.h"

#define BDD_DEDFAULT_ADDR 0x11

#define pacConfigReg            0x00
#define pacConverionReg         0x01
#define pacSourceSamplingReg    0x0A
#define pacSenseSamplingReg     0x0B

#define sensReg                 0x0D
#define sourceReg               0x11
#define powerReg                0x15

#define pacConfiguration    0b01100000
#define pacConversion       0b00
#define pacSourceSampling   0b00001100
#define pacSenseSampling    0b01110011

#define ch1     0
#define ch2     1
#define neg     0
#define pos     1

class BDD{

    public:
        /** Create an instance of a BDD module with specified parameters
         * 
         *  @param *boardI2C pointer to an I2C instance used to communicate
         *  @param ADDR module I2C address (default 0x10)
         */
        BDD(I2C* boardI2C, uint8_t ADDR = BDD_DEDFAULT_ADDR);
        status_t setTime(bool channel, bool polarity, long time);
        status_t setDuty(bool channel, uint8_t duty_cycle);
        status_t getTime(bool channel, bool polarity);
        status_t getDuty(bool channel);
        status_t getPol(bool channel);
        status_t pacInit(bool channel, float shunt, uint8_t ADDR);
        status_t getV(bool channel);
        status_t getI(bool channel);
        status_t getP(bool channel);

        void setI2C(I2C *boardI2C);
        void setAddress(uint8_t ADDR);

        float voltage(bool channel);
        float current(bool channel);
        float power(bool channel);
        long time(bool channel, bool polarity);
        uint8_t duty(bool channel);
        bool polarity(bool channel);

    private:
        channel_t ch[2];
        I2C *moduleI2C;
        uint8_t moduleAddress;
};

#endif