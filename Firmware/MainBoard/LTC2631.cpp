#include "LTC2631.h"

#include <Arduino.h>
#include <Wire.h>

LTC2631::LTC2631(uint8_t i2cAddress) :
    i2cAddress(i2cAddress)
{
}

void LTC2631::begin()
{
    writeAndUpdateDacRegister(0);
}

// Table 3. LTC2631 Command Codes
// COMMAND
// C3 C2 C1 C0
//  0  0  0  0   Write to Input Register
//  0  0  0  1   Update (Power Up) DAC Register
//  0  0  1  1   Write to and Update (Power Up) DAC Register
//  0  1  0  0   Power Down
//  0  1  1  0   Select Internal Reference
//  0  1  1  1   Select External Reference

LTC2631::ReturnCode LTC2631::writeInputRegister(uint16_t val)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x00);
    writeData(val);
    if (Wire.endTransmission())
        return ReturnCode::WriteError;
    else
        return ReturnCode::NoError;
}

LTC2631::ReturnCode LTC2631::updateDacRegister()
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x10);
    writeData(0);
    if (Wire.endTransmission())
        return ReturnCode::UpdateError;
    else
        return ReturnCode::NoError;
}

LTC2631::ReturnCode LTC2631::writeAndUpdateDacRegister(uint16_t val)
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x30);
    writeData(val);
    if (Wire.endTransmission())
        return ReturnCode::UpdateError;
    else
        return ReturnCode::NoError;
}

LTC2631::ReturnCode LTC2631::powerDown()
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x40);
    writeData(0);
    if (Wire.endTransmission())
        return ReturnCode::WriteError;
    else
        return ReturnCode::NoError;
}

LTC2631::ReturnCode LTC2631::selectInternalReference()
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x60);
    writeData(0);
    if (Wire.endTransmission())
        return ReturnCode::WriteError;
    else
        return ReturnCode::NoError;
}

LTC2631::ReturnCode LTC2631::selectIExternalReference()
{
    Wire.beginTransmission(i2cAddress);
    Wire.write(0x70);
    writeData(0);
    if (Wire.endTransmission())
        return ReturnCode::WriteError;
    else
        return ReturnCode::NoError;
}

void LTC2631::writeData(uint16_t data)
{
    Wire.write((data >> 4) & 0xFF);
    Wire.write((data << 4) & 0xFF);
}
