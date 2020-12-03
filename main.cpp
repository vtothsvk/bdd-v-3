#include "mbed.h"
#include "PAC.h"
#include "SHTC3.h"
#include "RTClib.h"
#include "yFlow.h"
#include "Pressure.h"
//#include "INA219.h"
#include "INA226.h"
#include <iostream>

#define CAN_ADDR    0x06
#define CAN_type    0x01

#define DEAD_TIME   3600

#define channels 2

#define ch1     0
#define ch2     1
#define v5      2
#define v12     3

#define negative    0
#define positive    1

//Polarity switch controll pins
#define polarityPin1    D12//PB_4
#define polarityPin2    A0//PA_0

//PWM controll pins
#define pwmPin1         A2//PA_3
#define pwmPin2         D6//PB_1

//Voltage measurement pins
#define voltagePin1     A3
#define voltagePin2     A6

#define flowPin         D7

#define defaultPolarity positive

//Channel output default settings
#define defaultPolTime  60//s
#define defaultPWM      0//(1-x)*100%
#define PWMperiod       10//ms

#define pacCal      2

#define DCId        0x00
#define polTimeId   0x08
#define HBId        0x10
#define SHTCId      0x18
#define IGBTId      0x1C
#define FlowId      0x1E
#define PressureId  0x1F
#define PolarityId  0x24

#define PACId       0x20

#define VId         0x10
#define IId         0x08
#define PId         0x04

#define RW          0x01
#define CH          0x02

#define WBADDR      0x00

//INA219 i2c address settings
#define INA1_ADDR  0x40
#define INA2_ADDR  0x45

float fround(float var) 
{
    float value = (int)(var * 100 + .5); 
    return (float)value / 100;
} 

typedef struct BDD_channel{
    float polTime = defaultPolTime, dc = defaultPWM;
    bool pol = false;
}channel_t;

/* *///Booting///* */
void BDD_init(void);
void CAN_init(void);

void LED(void);
void polaritySwitch(uint8_t channel);
void polaritySwitch1(void);
void polaritySwitch2(void);

void inaReset();

/* *///CAN callback///* */
void CAN_cb(void);
void CAN_incoming_data_cb(CANMessage *msg);
static status_t CAN_pub(CANMessage msg);

void blink(void);

/* *///Voltage measurement///* */
float getVoltage(AnalogIn* pin, uint16_t period);

/* *///CAN///* */
CAN can(PA_11, PA_12);
CANMessage msgBuffer;

/* *///I2C///* */
I2C i2c(D4, D5);

/* *///INA///* */
//INA219 ina[2]{INA219(&i2c, INA1_ADDR), INA219(&i2c, INA2_ADDR)};
static INA226 ina[2]{INA226(&i2c, INA1_ADDR), INA226(&i2c, INA2_ADDR)};

//GPIO//
//Heartbeat LED
DigitalOut led(LED1);

//BDD channel polarity controll pins
DigitalOut  polarity[2]{DigitalOut(polarityPin1), DigitalOut(polarityPin2)};

//BDD channel pwm controll pins
PwmOut pwm[2]{PwmOut(pwmPin1), PwmOut(pwmPin2)};

//Flowmeter input pin
InterruptIn flowmeter(flowPin);

//Utility button
InterruptIn button(D11);

//BDD channel voltage measurement inputs
AnalogIn pac[2]{AnalogIn(voltagePin1), AnalogIn(voltagePin2)};

//Temporary channel 2 SSR relay control
DigitalOut tempPol(D12);

/* *///Timers///* */
Ticker polarityTimer[2];
Ticker LEDtim;
Ticker inaT;

/* *///CAN Thread///* */
EventQueue CAN_cb_queue;
Thread CANThread;

/* *///RTC///* */
pcfRTC rtc(&i2c);

/* *///SHTC3 temp&hum///* */
SHTC shtc(&i2c);

/* *///y820 Flowmeter///* */
yFlow flow(&flowmeter);

/* *///BDD module variables///* */
channel_t channel[2];
//PAC pac[4]{PAC(&i2c, PAC_DEFAULT_ADDR + 2), PAC(&i2c, PAC_DEFAULT_ADDR + 3), PAC(&i2c, PAC_DEFAULT_ADDR), PAC(&i2c, PAC_DEFAULT_ADDR + 1)};
static int elapsed = 0;
static bool isInaRSTPending = false;

int main(void){
    ThisThread::sleep_for(100);
    cout << "Booting BDD module..." << endl;
    //inaT.attach(&inaReset, 30);

    //booting sequence
    //LEDtim.attach(&LED, 1);
    //button.rise(&cvak);
    tempPol = 0;

    cout << "INA 1 reset: " << ina[0].reset() << endl;
    cout << "INA 2 reset: " << ina[1].reset() << endl;

    
    if(rtc.init()){
        cout << "rtc failed to initialise..." << endl;
    }
    if(shtc.init()){
        cout << "shtc failed to initialise..." << endl;
    }
    if(ina[0].init()){
        cout << "INA 1 failed to initialise..." << endl;
    }
    if(ina[1].init()){
        cout << "INA 2 failed to initialise..." << endl;
    }
    //BDD_init();
    //CAN_init();

    cout << "BDD module booting sequence complete" << endl;
    //end of booting sequence
    
    while(elapsed <= DEAD_TIME){
        
        ThisThread::sleep_for(1000);

        if(isInaRSTPending){
            for(uint8_t dz = 0; dz < 2; dz++){
                ina[dz].reset();
                ina[dz].init();
                isInaRSTPending = false;
            }//for end
        }//if(isInaRSTPending)

        for (uint8_t i = 0; i < 2; i++){
            cout << "ina read status: " << ina[i].readI() << endl;
            float volt = getVoltage(&pac[i], PWMperiod);
            cout << "CH" << i + 0 << ": " << volt + 0.0 << "V, " << ina[i].current() << "mA" << endl;
        }//for end
        
        elapsed++;
    }//loop end

    NVIC_SystemReset();
}//main

void polaritySwitch(uint8_t ch){
    pwm[ch] = 1;
    polarity[ch] = !polarity[ch];
    channel[ch].pol = !channel[ch].pol;
    pwm[ch] = channel[ch].dc;
    isInaRSTPending = true;
}//polaritySwitch

void polaritySwitch1(){
    pwm[ch1] = 1;
    polarity[ch1] = !polarity[ch1];
    channel[ch1].pol = !channel[ch1].pol;
    pwm[ch1] = channel[ch1].dc;
    isInaRSTPending = true;
}//polaritySwitch1

void polaritySwitch2(){
    pwm[ch2] = 1;
    polarity[ch2] = !polarity[ch2];
    channel[ch2].pol = !channel[ch2].pol;
    pwm[ch2] = channel[ch2].dc;
    isInaRSTPending = true;
}//polaritySwitch2

void BDD_init(void){
    for(uint8_t i = 0; i < channels; i++){
        pwm[i].period_ms(PWMperiod);
        pwm[i] = defaultPWM;
        polarity[i] = defaultPolarity;
    }
    for(uint8_t i = 0; i < 4; i++){
        if(0){//pac[i].init()){
            cout << "PAC " << i + 0 << " failed to initialise..." << endl;
        }//if end
    }//for end

    //pwm[0] = 1;
    //pwm[1] = 1;
    polarityTimer[ch1].attach(&polaritySwitch1, defaultPolTime);
    polarityTimer[ch2].attach(&polaritySwitch2, defaultPolTime);
}//BDD_init

void CAN_init(){
    int canID = 0x380;
    can.frequency(250000);
    CANThread.start(callback(&CAN_cb_queue, &EventQueue::dispatch_forever));
    can.filter(canID, 0x7C0, CANStandard);
    can.attach(CAN_cb_queue.event(&CAN_cb), CAN::RxIrq);
}//CAN_init

void CAN_cb(){
    if(can.read(msgBuffer, 0)){
        elapsed = 0;
        CAN_cb_queue.call(&CAN_incoming_data_cb, &msgBuffer);
    }
    blink();
}//CAN_cb

void CAN_incoming_data_cb(CANMessage *msg){
    uint8_t commandId = msg -> id;

    if((commandId & PressureId) == PressureId){
        //Pressure (R)
    }//Pressure

    else if((commandId & FlowId) == FlowId){
        //FLow (R)
        flow.measure();
        char buffer[4];
        float lastFlow = flow.lastFlow();
        memcpy(&buffer[0], &lastFlow, 4);
        CAN_cb_queue.call(&CAN_pub, CANMessage(FlowId, &buffer[0], 4, CANData, CANStandard));
    }//Flow

    else if((commandId & IGBTId) == IGBTId){
        //IGBT Temp (R)

    }//IGBT Temp

    else if((commandId & SHTCId) == SHTCId){
        //SHTC Temp (R)
        shtc.read();
        char buffer[8];
        float ambientTemp = shtc.lastTemp();
        float ambientHum = shtc.lastHum();

        memcpy(&buffer[0], &ambientTemp, 4);
        memcpy(&buffer[4], &ambientHum, 4);

        CAN_cb_queue.call(&CAN_pub, CANMessage(SHTCId, &buffer[0], 8, CANData, CANStandard));
    }//SHTC

    else if((commandId & HBId) == HBId){
        //H-bridge control (R/W)
    }//H-bridge

    else if ((commandId & polTimeId) == polTimeId){
        //Polarity Time (R/W)
        if(commandId & RW){
            //Read
            char buffer[8];
            memcpy(&buffer[0], &channel[ch1].polTime, 4);
            memcpy(&buffer[4], &channel[ch2].polTime, 4);
            CAN_cb_queue.call(&CAN_pub, CANMessage(polTimeId + 1, &buffer[0], 8, CANData, CANStandard));
        }//if R/W
        else{
            //Write
            if(commandId & CH){
                channel[ch2].polTime = *(float*)&msg->data[0];
                polarityTimer[ch2].attach(&polaritySwitch2, channel[ch2].polTime);
            }else{
                channel[ch1].polTime = *(float*)&msg->data[0];
                polarityTimer[ch1].attach(&polaritySwitch1, channel[ch1].polTime);
            }
        }//else R/W
    }//PolTime

    else if((commandId & PACId) == PACId){
        //VIP (R)
        uint8_t pacId = commandId&0b11;

        char buffer[8];

        float v = getVoltage(&pac[pacId], PWMperiod);
        float i = ina[pacId].current();
        cout << "sending i : " << i + 0.0 << endl;

        memcpy(&buffer[0], &v, 4);
        memcpy(&buffer[4], &i, 4);

        CAN_cb_queue.call(&CAN_pub, CANMessage(PACId|pacId, &buffer[0], 8, CANData, CANStandard));
    }//VIP

    else if((commandId & PolarityId) == PolarityId){
        char buffer[2];
        buffer[0] = (uint8_t)channel[ch1].pol;
        buffer[1] = (uint8_t)channel[ch2].pol;

        cout << "polarities: " << channel[0].pol << ", " << channel[1].pol << endl;

        CAN_cb_queue.call(&CAN_pub, CANMessage(PolarityId, &buffer[0], 2, CANData, CANStandard));
    }//Polarity

    else if((commandId & DCId) == DCId){
        //Duty CCycle (R/W)
        if(commandId & RW){
            //Read
            char buffer[8];
            memcpy(&buffer[0], &channel[ch1].dc, 4);
            memcpy(&buffer[4], &channel[ch2].dc, 4);
            CAN_cb_queue.call(&CAN_pub, CANMessage(DCId + 1, &buffer[0], 8, CANData, CANStandard));
        }//if R/W
        else{
            //Write
            channel[(commandId & CH) >> 1].dc = 1 - (*(float*)&(msg -> data[0]));
            pwm[(commandId & CH) >> 1] = channel[(commandId & CH) >> 1].dc;
            cout << "dc " << ((commandId & CH) >> 1) << " set to: " << channel[(commandId & CH) >> 1].dc << endl;
        }//else R/W
    }//Duty cycle

    else{ cout << "Unknown command.." << endl;} 
}//CAN_incoming_data_cb

status_t CAN_pub(CANMessage msg){
    if(can.write(msg)){
        return STATUS_TX_ERROR;
    }
    return STATUS_OK;
}//CAN_pub

void LED(void){
    led = !led;
}//LED

void blink(void){
    led = 1;
    ThisThread::sleep_for(25);
    led = 0;
    ThisThread::sleep_for(25);
    led = 1;
    ThisThread::sleep_for(25);
    led = 0;
}//blink

float getVoltage(AnalogIn* pin, uint16_t period){
    float buffer;
    for(uint8_t i = 0; i < 20; i++){
        buffer += (pin -> read()*33);
        ThisThread::sleep_for((uint32_t)(period/10));
    }
    return (fround(buffer / 20));
}//getVoltage

void inaReset(){
    isInaRSTPending = true;
}//inaReset