#include "yFlow.h"

yFlow::yFlow(InterruptIn *pin){
    this -> flowPin = pin;
}

void yFlow::measure(){
    this -> pulse = 0;
    flowPin -> rise(this, &yFlow::rpm);
    ThisThread::sleep_for(1000);
    flowPin -> rise(NULL);

    this -> flow = (this -> pulse) / 7.5;
}

float yFlow::lastFlow(){
    return this -> flow;
}

void yFlow::rpm(){
    this -> pulse++;
}