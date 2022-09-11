#pragma once

#include <MCP342x.h>

const int MCP3428_ADRESS = 0x68;

enum class Output {
    VoltageA,
    CurrentA,
    VoltageB,
    CurrentB
};

class OutputSensor {
public:
    OutputSensor(HardwareSerial &mySerial);
    void begin();
    void sample();

    int readOutput(Output sel);

private:
    long sampleChannel(MCP342x::Channel ch);

private:
    MCP342x adc;
    HardwareSerial &mySerial;

    int chAVoltage1000x;
    int chACurrent1000x;
    int chBVoltage1000x;
    int chBCurrent1000x;
};

