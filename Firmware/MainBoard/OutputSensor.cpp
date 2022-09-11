#include "OutputSensor.h"

#include <Wire.h>

OutputSensor::OutputSensor(HardwareSerial &mySerial) :
    adc(MCP3428_ADRESS),
    mySerial(mySerial)
{ }

void OutputSensor::begin()
{
    // Reset devices
    MCP342x::generalCallReset();
    delay(1); // MC342x needs 300us to settle, wait 1ms
    // Check device present
    Wire.requestFrom(MCP3428_ADRESS, (uint8_t)1);
    if (!Wire.available()) {
        mySerial.print("No device found at address 0x");
        mySerial.println(MCP3428_ADRESS, HEX);
    } else {
        mySerial.print("Found MCP3428 at address 0x");
        mySerial.println(MCP3428_ADRESS, HEX);
    }
}

long OutputSensor::sampleChannel(MCP342x::Channel ch)
{
    long value = 0;
    MCP342x::Config status;
    // Initiate a conversion; convertAndRead() will wait until it can be read
    uint8_t err = adc.convertAndRead(
        ch, MCP342x::oneShot,
        MCP342x::resolution16, MCP342x::gain1,
        1000000, value, status
    );
    if (err) {
        mySerial.print("Error ");
        mySerial.print(err);
        mySerial.println(" sampling MCP3428");
    // } else {
    //     mySerial.print("Sampled value: ");
    //     mySerial.println(value);
    }
    return value;
}


void OutputSensor::sample()
{
    chAVoltage1000x = sampleChannel(MCP342x::channel1);
    chACurrent1000x = sampleChannel(MCP342x::channel2);
    chBVoltage1000x = sampleChannel(MCP342x::channel3);
    chBCurrent1000x = sampleChannel(MCP342x::channel4);
}

int OutputSensor::readOutput(Output sel)
{
    switch (sel) {
    case Output::VoltageA:
        return chAVoltage1000x;
    case Output::CurrentA:
        return chACurrent1000x;
    case Output::VoltageB:
        return chBVoltage1000x;
    case Output::CurrentB:
        return chBCurrent1000x;
    }
}
