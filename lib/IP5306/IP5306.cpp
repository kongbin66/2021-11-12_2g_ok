
#include "IP5306.h"
void IP5306::begin(uint8_t addr,TwoWire &wirePort) {
     _addr = addr;
     _i2cPort = &wirePort;
}

uint8_t IP5306::writeBytes(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t quantity) {
    _i2cPort->beginTransmission(addr);
    _i2cPort->write(reg);
    _i2cPort->write(data, quantity);
    return _i2cPort->endTransmission() == I2C_ERROR_OK;

}
uint8_t IP5306::readBytes(uint8_t addr, uint8_t reg, uint8_t *data, uint8_t quantity) {
    _i2cPort->beginTransmission(addr);
    _i2cPort->write(reg);
    if (_i2cPort->endTransmission(false) != I2C_ERROR_OK) { return 0; }

    _i2cPort->requestFrom(addr, quantity);
    uint8_t i = 0;
    while (_i2cPort->available() && i < quantity) {
        data[i] = _i2cPort->read();
        i++;
    }
    while (_i2cPort->available()) {
        _i2cPort->read(); 
    }
    return i == quantity;
}

uint8_t IP5306::setup(uint8_t initPrms ) {
    return this->writeBytes(IP5306_ADDR, IP5306_REG_SYS_CTL0, &initPrms, 1);
}

uint8_t IP5306::isChargerConnected() {
    uint8_t data;
    this->readBytes(IP5306_ADDR, IP5306_REG_READ0, &data, 1);
    return (data & (1 << 3)) ? 1 : 0;
}

uint8_t IP5306::isChargeFull() {
    uint8_t data;
    this->readBytes(IP5306_ADDR, IP5306_REG_READ1, &data, 1);
    return (data & (1 << 3)) ? 1 : 0;
}

uint8_t IP5306::getBatteryLevel() {
    uint8_t data;
    this->readBytes(IP5306_ADDR, IP5306_REG_UNKNOW, &data, 1);
    switch (data & 0xF0) {
        case 0xE0: return 25;
        case 0xC0: return 50;
        case 0x80: return 75;
        case 0x00: return 100;
        default: return 0;
    }    
}

 uint8_t IP5306::setPowerBoostKeepOn(uint8_t en)
{
    uint8_t data;
    this->readBytes(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data, 1);
    if(en) data|=(1<<1);
    else data&=~(1<<1);
    return this->writeBytes(IP5306_ADDR, IP5306_REG_SYS_CTL0, &data, 1);
} 

