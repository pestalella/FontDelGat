#pragma once

#include "OutputSensor.h"

#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

class BoardControl {
    enum Command {
        NO_COMMAND,
        CMD_SET_INDEPENDENT,
        CMD_SET_PARALLEL,
        CMD_SET_SERIES
    };

    enum OutputMode {
        MODE_INDEPENDENT,
        MODE_PARALLEL,
        MODE_SERIES
    };

public:
    BoardControl();
    void begin();
    void checkFan();
    void checkCommands();
    void updateOutputInfo();
    void updateOutputVoltages();

private:
    Command readSerialCommand();
    void changeOutputMode(OutputMode newOutputState);

private:
    HardwareSerial mySerial;
    OutputSensor outSense;
    LiquidCrystal_I2C lcd;

    OutputMode outputState;
};
