#include <Arduino.h>
#include <HardwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

const int PARALLEL_OUT = PB13;
const int SERIES_OUT = PB12;
// const int CHANNEL_A_EN = PA11;
const int CHANNEL_B_EN = PB15;
const int V_TEMP_A = PA0;
const int V_TEMP_B = PA1;
const int FAN_CTRL = PC6;

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

LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display
HardwareSerial mySerial(PA10, PA9);

int count = 2;
bool fanRunning = false;
int fanDelay = 0;

void initPins()
{
    pinMode(CHANNEL_B_EN, OUTPUT);
    pinMode(PARALLEL_OUT, OUTPUT);
    pinMode(SERIES_OUT, OUTPUT);
    pinMode(FAN_CTRL, OUTPUT);

    digitalWrite(CHANNEL_B_EN, LOW);
    digitalWrite(PARALLEL_OUT, LOW);
    digitalWrite(SERIES_OUT, LOW);
    digitalWrite(FAN_CTRL, LOW);

    lcd.begin();
    // Print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(3,0);
    lcd.print("Hello, world!");
    delay(1000);
    lcd.clear();

    mySerial.begin(115200);
    mySerial.println ("Hello!");

}

void checkFan()
{
    String msgLine;
    char tempMsgLine[20];

    int tempSensorValueA = analogRead(V_TEMP_A);
    int tempSensorValueB = analogRead(V_TEMP_B);

    int tempA = (tempSensorValueA*22)/79;
    int tempB = (tempSensorValueB*22)/79;

    lcd.setCursor(0,0);
    msgLine = "TA: " + String(tempA);
    msgLine.toCharArray(tempMsgLine, 20);
    lcd.print(tempMsgLine);
    lcd.setCursor(10,0);
    msgLine = "TB: " + String(tempB);
    msgLine.toCharArray(tempMsgLine, 20);
    lcd.print(tempMsgLine);
    bool shouldFanRun = (tempA > 30 || tempB > 30);
    if (fanDelay == 0) {
        if (shouldFanRun != fanRunning) {
            fanRunning = shouldFanRun;
            fanDelay = 50;
            digitalWrite(FAN_CTRL, fanRunning);
        }
    } else {
        fanDelay--;
    }
}

Command readSerialCommand()
{
    int c  = mySerial.read();

    Command cmd;
    switch (c) {
        case -1:
            cmd = NO_COMMAND;
            break;
        case 'i':
        case 'I':
            mySerial.println("Set independent outputs");
            cmd = CMD_SET_INDEPENDENT;
            break;
        case 'p':
        case 'P':
            mySerial.println("Set parallel outputs");
            cmd = CMD_SET_PARALLEL;
            break;
        case 's':
        case 'S':
            mySerial.println("Set serial outputs");
            cmd = CMD_SET_SERIES;
            break;
        default:
            cmd = NO_COMMAND;
            break;
    }
    return cmd;
}

OutputMode outputState;

void changeOutputMode(OutputMode newOutputState)
{
    // Disable relays first, short-circuits are dangerous
    digitalWrite(PARALLEL_OUT, LOW);
    digitalWrite(SERIES_OUT, LOW);
    // Wait for relays to stabilize
    delay(50);
    // Change to new mode
    switch (newOutputState) {
        case MODE_INDEPENDENT:
            break;
        case MODE_PARALLEL:
            digitalWrite(PARALLEL_OUT, HIGH);
            break;
        case MODE_SERIES:
            digitalWrite(SERIES_OUT, HIGH);
            break;
    }
}

void checkCommands()
{
    Command cmd = readSerialCommand();
    OutputMode newOutputState;
    switch (cmd) {
        case NO_COMMAND:
            newOutputState = outputState;
            break;
        case CMD_SET_INDEPENDENT:
            newOutputState = MODE_INDEPENDENT;
            break;
        case CMD_SET_PARALLEL:
            newOutputState = MODE_PARALLEL;
            break;
        case CMD_SET_SERIES:
            newOutputState = MODE_SERIES;
            break;
    }
    if (newOutputState != outputState) {
        changeOutputMode(newOutputState);
        outputState = newOutputState;
    }
}
