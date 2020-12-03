#ifndef RTClib_h
#define RTClib_h

#include "mbed.h"
#include "drivers/I2C.h"
#include <cstdlib>
#include <iostream>
#include <ctime>

#ifndef RTC_LIB_YEAR_OFFSET
#define RTC_LIB_YEAR_OFFSET
#define yrOffset 1900
#endif

#define rtcAddr     (0x68 << 1)
#define initReg     0x02
#define timeReg     0x03

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

static uint8_t bin2bcd (uint8_t val) { return (val + 6 * (val / 10)); }
static uint8_t bcd2bin (uint8_t val) { return (val - 6 * (val >> 4)); }

/*class rtcTime{
public:
    rtcTime(uint8_t ss = 0, uint8_t mm = 0, uint8_t hh = 0, uint8_t dd = 0, uint8_t wd = 0, uint8_t mon = 0, uint8_t yy = 0);
    void setTime(uint8_t ss, uint8_t mm, uint8_t hh, uint8_t dd, uint8_t wd, uint8_t mon, uint8_t yy);
    void set(long epoch);
    void time();
    void printTime();
    long unix();
    uint8_t seconds();
    uint8_t minutes();
    uint8_t hours();
    uint8_t days();
    uint8_t weekdays();
    uint8_t months();
    uint16_t year();
    uint8_t ss;
    uint8_t mm;
    uint8_t hh;
    uint8_t dd;
    uint8_t wd;
    uint8_t mon;
    uint8_t yy;

    inline rtcTime& operator= (long x) __attribute__((always_inline)){
        struct tm t;
        time_t epoch = x;
        t = *localtime(&epoch);
        this -> ss = bin2bcd(t.tm_sec);
        this -> mm = bin2bcd(t.tm_min);
        this -> hh = bin2bcd(t.tm_hour);
        this -> dd = bin2bcd(t.tm_mday);
        this -> wd = bin2bcd(t.tm_wday);
        this -> mon = bin2bcd(t.tm_mon);
        this -> yy = bin2bcd(t.tm_year);
        return *this;
    }
};*/

class pcfRTC{
public:
    pcfRTC(I2C *boardI2C);
    status_t init();
    status_t set(time_t t);
    status_t read();
    unsigned long lastT();
    void printTime();
private:
    I2C *rtcI2C;
    struct tm last;
};

#endif RTClib_h