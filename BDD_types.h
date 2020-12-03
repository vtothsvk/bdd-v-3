#ifndef BDD_types_h
#define BDD_types_h

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

typedef struct BDD_channel{
    float v, i, p, shunt;
    long time[2];
    uint8_t reg, dc, pacAddr;
    bool pol;
}channel_t;

typedef union bdd_data{
    char dataBuff[4];
    float data;
}bdd_data_t;

typedef union transferData{
    char dataBuff[4];
    unsigned long data;
}bdd_transfer_buffer_t;
#endif