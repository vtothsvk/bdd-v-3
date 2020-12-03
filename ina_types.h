#ifndef ina_types_h
#define ina_types_h

#include <cstdint>

#define DEFAULT_CALIBRATION 2048//LSB=1mA @ .02R shunt

#define RES_9b  0
#define RES_10b 1
#define RES_11b 2
#define RES_12b 3
#define DEFAULT_RESOLUTION  RES_12b

#define INA219_CONF_REG     0x00
#define INA219_I_REG        0x04
#define INA219_CAL_REG      0x05


#define INA219_CONFIG_BVOLTAGERANGE_MASK        0x2000  // Bus Voltage Range Mask
#define INA219_CONFIG_BVOLTAGERANGE_16V         0x0000  // 0-16V Range
#define INA219_CONFIG_BVOLTAGERANGE_32V         0x2000  // 0-32V Range

#define INA219_CONFIG_GAIN_MASK                 0x1800  // Gain Mask
#define INA219_CONFIG_GAIN_1_40MV               0x0000  // Gain 1, 40mV Range
#define INA219_CONFIG_GAIN_2_80MV               0x0800  // Gain 2, 80mV Range
#define INA219_CONFIG_GAIN_4_160MV              0x1000  // Gain 4, 160mV Range
#define INA219_CONFIG_GAIN_8_320MV              0x1800  // Gain 8, 320mV Range

#define INA219_CONFIG_BADCRES_MASK              0x0780  // Bus ADC Resolution Mask
#define INA219_CONFIG_BADCRES_9BIT              0x0080  // 9-bit bus res = 0..511
#define INA219_CONFIG_BADCRES_10BIT             0x0100  // 10-bit bus res = 0..1023
#define INA219_CONFIG_BADCRES_11BIT             0x0200  // 11-bit bus res = 0..2047
#define INA219_CONFIG_BADCRES_12BIT             0x0400  // 12-bit bus res = 0..4097

#define INA219_CONFIG_SADCRES_MASK              0x0078  // Shunt ADC Resolution and Averaging Mask
#define INA219_CONFIG_SADCRES_9BIT_1S_84US      0x0000  // 1 x 9-bit shunt sample
#define INA219_CONFIG_SADCRES_10BIT_1S_148US    0x0008  // 1 x 10-bit shunt sample
#define INA219_CONFIG_SADCRES_11BIT_1S_276US    0x0010  // 1 x 11-bit shunt sample
#define INA219_CONFIG_SADCRES_12BIT_1S_532US    0x0018  // 1 x 12-bit shunt sample
#define INA219_CONFIG_SADCRES_12BIT_2S_1060US   0x0048  // 2 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_4S_2130US   0x0050  // 4 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_8S_4260US   0x0058  // 8 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_16S_8510US  0x0060  // 16 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_32S_17MS    0x0068  // 32 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_64S_34MS    0x0070  // 64 x 12-bit shunt samples averaged together
#define INA219_CONFIG_SADCRES_12BIT_128S_69MS   0x0078  // 128 x 12-bit shunt samples averaged together

#define INA219_CONFIG_MODE_MASK                 0x0007  // Operating Mode Mask
#define INA219_CONFIG_MODE_POWERDOWN            0x0000
#define INA219_CONFIG_MODE_SVOLT_TRIGGERED      0x0001
#define INA219_CONFIG_MODE_BVOLT_TRIGGERED      0x0002
#define INA219_CONFIG_MODE_SANDBVOLT_TRIGGERED  0x0003
#define INA219_CONFIG_MODE_ADCOFF               0x0004
#define INA219_CONFIG_MODE_SVOLT_CONTINUOUS     0x0005
#define INA219_CONFIG_MODE_BVOLT_CONTINUOUS     0x0006
#define INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS 0x0007

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