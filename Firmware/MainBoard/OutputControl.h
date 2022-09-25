#pragma once

#include <MCP4725.h>


class OutputControl {
public:
    enum class Channel {
        A,
        B
    };

    OutputControl(HardwareSerial &mySerial);
    void begin();
    int setOutput(Channel chSel, int voltagex1000);

private:
    const int CHANNEL_A_DAC_ADDRESS = 0x10;
    const int CHANNEL_B_DAC_ADDRESS = 0x12;

    // MCP4725 dacChannelA;
    // MCP4725 dacChannelB;
    HardwareSerial &mySerial;
};
