#pragma once

#include <Arduino.h>

// LTC2631CTS8-HZ12
// Part number: LTC2631
// Temperature grade: C = 0ºC - 70ºC
// Package type: TS8 = 8-Lead Plastic TSOT-23
// Full scale voltage: H = 4.096 V
// Power-on reset: Z = Reset to Zero-scale
// Resolution: 12 = 12-bit


class LTC2631
{
public:
    enum class ReturnCode {
        NoError,
        WriteError,
        UpdateError
    };

public:
    LTC2631(uint8_t i2cAddress);
    void begin();
    ReturnCode writeInputRegister(uint16_t val);
    ReturnCode updateDacRegister();
    ReturnCode writeAndUpdateDacRegister(uint16_t val);
    ReturnCode powerDown();
    ReturnCode selectInternalReference();
    ReturnCode selectIExternalReference();

private:
    void writeData(uint16_t data);

private:
    uint8_t i2cAddress;
};
