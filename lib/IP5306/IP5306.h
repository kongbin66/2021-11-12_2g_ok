#ifndef IP5306_H
#define IP5306_H
#include <Wire.h>

#define IP5306_DEFAULT_SETUP    0x37  // set power boost keep on by default
#define IP5306_ADDR             0x75
#define IP5306_REG_SYS_CTL0     0x00  // initialize
#define IP5306_REG_READ0        0x70  // charge en
#define IP5306_REG_READ1        0x71  // charge full
#define IP5306_REG_UNKNOW       0x78  // bat level

class IP5306 {
    
    public:
        void begin( uint8_t addr=IP5306_ADDR ,TwoWire &wirePort=Wire);
        uint8_t writeBytes(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t quantity);
        uint8_t readBytes(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t quantity);
        uint8_t setup(uint8_t initPrms = IP5306_DEFAULT_SETUP);
        uint8_t isChargerConnected();//连接充电线
        uint8_t isChargeFull();//电池充满
        uint8_t getBatteryLevel();//获得电量
        uint8_t setPowerBoostKeepOn(uint8_t en=1);//设置保持升压。
    private:
        uint8_t _addr;
        TwoWire *_i2cPort;
};

#endif