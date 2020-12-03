#ifndef INA226_types_h
#define INA226_types_h

#include <cstdint>

#define DEFAULT_CALIBRATION 256//LSB = 1mA @ .02R shunt

/* *///Resolution///* */
#define RES_9b  0
#define RES_10b 1
#define RES_11b 2
#define RES_12b 3
#define RES_13b 4
#define RES_14b 5
#define RES_15b 6
#define RES_16b 7

#define DEFAULT_RESOLUTION  RES_12b

#define INA226_CONF_REG     0x00
#define INA226_V_REG        0x02
#define INA226_I_REG        0x04
#define INA226_CAL_REG      0x05

/* *///Configuration//* */
#define INA226_CONFIG_RESET                 0x8000

//Averaging
#define INA226_CONFIG_AVERAGING_1           0x0000
#define INA226_CONFIG_AVERAGING_4           0x0200
#define INA226_CONFIG_AVERAGING_16          0x0400
#define INA226_CONFIG_AVERAGING_64          0x0600
#define INA226_CONFIG_AVERAGING_129         0x0800
#define INA226_CONFIG_AVERAGING_256         0x0A00
#define INA226_CONFIG_AVERAGING_512         0x0C00
#define INA226_CONFIG_AVERAGING_1024        0x0E00

#define DEFAULT_AVERAGED_S  INA226_CONFIG_AVERAGING_129

//Bus voltage conversion time
#define INA226_CONFIG_VBUS_140              0x0000//9b ADC
#define INA226_CONFIG_VBUS_204              0x0040//10b ADC
#define INA226_CONFIG_VBUS_332              0x0080//11b ADC
#define INA226_CONFIG_VBUS_588              0x00C0//12b ADC
#define INA226_CONFIG_VBUS_1100             0x0100//13b ADC
#define INA226_CONFIG_VBUS_2116             0x0140//14b ADC
#define INA226_CONFIG_VBUS_4156             0x0180//15b ADC
#define INA226_CONFIG_VBUS_8244             0x01C0//16b ADC

//Shunt voltage conversion time
#define INA226_CONFIG_SHUNT_140             0x0000//9b ADC
#define INA226_CONFIG_SHUNT_204             0x0008//10b ADC
#define INA226_CONFIG_SHUNT_332             0x0010//11b ADC
#define INA226_CONFIG_SHUNT_588             0x0018//12b ADC
#define INA226_CONFIG_SHUNT_1100            0x0020//13b ADC
#define INA226_CONFIG_SHUNT_2116            0x0028//14b ADC
#define INA226_CONFIG_SHUNT_4156            0x0030//15b ADC
#define INA226_CONFIG_SHUNT_8244            0x0038//16b ADC

//Mode
#define INA226_CONFIG_MODE_P_DOWN           0x0000
#define INA226_CONFIG_MODE_SH_V_TRIG        0x0001
#define INA226_CONFIG_MODE_BUS_V_TRIG       0x0002
#define INA226_CONFIG_MODE_SH_N_BUS_TRIG    0x0003
#define INA226_CONFIG_MODE_P_DOWN2          0x0004
#define INA226_CONFIG_MODE_SH_V_CONT        0x0005
#define INA226_CONFIG_MODE_BUS_V_CONT       0x0006
#define INA226_CONFIG_MODE_SH_N_BUS_V_CONT  0x0007

#define INA266_DEFAULT_MODE INA226_CONFIG_MODE_SH_N_BUS_V_CONT

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

typedef struct ina_data{
    union{
        int rawCurrent;
        char rawCurrent_c[2];
    };
    union{
        int rawVoltage;
        char rawVoltage_c[2];
    };
    float lastCurrent, lastVoltage;
}ina_data_t;

#endif