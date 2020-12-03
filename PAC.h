/* Slovak Diamond Group
 * Faculty of Electrical Engineering andd Information technology 
 * Slovak University of Technology
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef PAC_h
#define PAC_h

#define PAC_DEFAULT_ADDR    0b1001100

#ifndef PAC_DEFAULT_SHUNT
#define PAC_DEFAULT_SHUNT 0.02
#endif

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

#include "mbed.h"
#include <cstdint>

#ifndef status_type
#define status_type
typedef enum status{
    STATUS_OK = 0,
    STATUS_NO_DATA = -1000,
    STATUS_TX_ERROR = -1001,
    STATUS_RX_ERROR = -1002,
    STATUS_RESERVED = -1003,
    STATUS_RESERVED2 = -1004,
    STATUS_UNKNOWN = -1005
}status_t;
#endif

/** Control interface of a PAC1710 bidirectional current sensor
 * 
 * @note Synchronisation level: Thread safe
 * 
 * Example:
 * @code
 * Set up a PAC1710 and periodically read V, I, P
 * #include "mbed.h"
 * #include "PAC.h"
 * #include <iostream>
 * 
 * #define PAC_ADDR     0b1001100
 * #define PAC_SHUNT    0.02
 * 
 * #define I2C_SDA_PIN PB_7
 * #define I2C_SCL_PIN PB_6
 * 
 * I2C i2c(I2C_SDA_PIN, I2C_SCL_PIN);
 * 
 * PAC pac(&i2c, PAC_ADDR, PAC_SHUNT);
 * 
 * int main(void){
 *     pac.init();
 *      
 *     while(true){
 *         pac.getV();
 *         pac.getI();
 *         pac.getP();
 *         
 *         cout << "V: " << pac.voltage() << endl;
 *         cout << "I: " << pac.current() << endl;
 *         cout << "P: " << pac.power() << endl;
 *         
 *         ThisThread::sleep_for(1000);
 *     }
 * }
 */
class PAC{
public:
    PAC(I2C *boardI2C, uint8_t ADDR = PAC_DEFAULT_ADDR, float shunt = PAC_DEFAULT_SHUNT);
    status_t init(void);
    status_t getV(void);
    status_t getI(void);
    status_t getP(void);
    float voltage(void);
    float current(void);
    float power(void);

private:
    I2C *pacI2C;
    float shunt, v, i, p;
    uint8_t pacAddr;
};

#endif