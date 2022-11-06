#pragma once

#include <Arduino.h>

class FrontPanel
{
public:
    FrontPanel();
    void begin();
    void update();

private:
    uint16_t readButtonState(int buttonNum);
    uint16_t scanKeys();
    void updateLeds();

private:
    const int INDEP_CHANNEL_EN = PA4;
    const int PAR_CHANNEL_EN = PA5;
    const int SERIES_CHANNEL_EN = PA6;
    const int CHANNEL_A_EN = PA11;
    const int CHANNEL_B_EN = PA12;
    const int I2C_SCL = PB8;
    const int I2C_SDA = PB9;
    const int S0 = PA0;
    const int S1 = PA13;
    const int S2 = PA1;
    const int S3 = PB3;
    const int COM = PB0;
    const int N_SCAN_EN = PA7;
};
