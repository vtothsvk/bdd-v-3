#ifndef yFlow_h
#define yFlow_h

#include "mbed.h"

class yFlow{
public:
    yFlow(InterruptIn *pin);
    void measure(void);
    float lastFlow();

private:
    void rpm();

    InterruptIn *flowPin;
    int pulse = 0;
    float flow;
};

#endif