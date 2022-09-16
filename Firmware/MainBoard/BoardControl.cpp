#include "BoardControl.h"

#include <Arduino.h>

const int PARALLEL_OUT = PB13;
const int SERIES_OUT = PB12;
// const int CHANNEL_A_EN = PA11;
const int CHANNEL_B_EN = PB15;
const int V_TEMP_A = PA0;
const int V_TEMP_B = PA1;
const int FAN_CTRL = PC6;
const int I2C_SDA = PB7;
const int I2C_SCL = PB6;

bool fanRunning = false;
int fanDelay = 0;

BoardControl::BoardControl() :
    mySerial(PA10, PA9),
    outSense(mySerial),
    lcd(0x27, 20, 4) // set the LCD address to 0x27 for a 20 chars and 4 line display
{
}

void BoardControl::begin()
{
    Wire.setSCL(I2C_SCL);
    Wire.setSDA(I2C_SDA);

    mySerial.begin(115200);
    mySerial.println("Board Control Firmware");

    Wire.begin();
    lcd.begin();

    pinMode(CHANNEL_B_EN, OUTPUT);
    pinMode(PARALLEL_OUT, OUTPUT);
    pinMode(SERIES_OUT, OUTPUT);
    pinMode(FAN_CTRL, OUTPUT);

    digitalWrite(CHANNEL_B_EN, LOW);
    digitalWrite(PARALLEL_OUT, LOW);
    digitalWrite(SERIES_OUT, LOW);
    digitalWrite(FAN_CTRL, LOW);

    outSense.begin();

    // Print a message to the LCD.
    lcd.backlight();
    lcd.setCursor(0,1);
    lcd.print("La Font del Gat v1.0");
    lcd.setCursor(1,2);
    lcd.print("(c) Pau Estalella");
    delay(3000);
    lcd.clear();
}

void BoardControl::checkFan()
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

BoardControl::Command BoardControl::readSerialCommand()
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

void BoardControl::changeOutputMode(OutputMode newOutputState)
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

void BoardControl::checkCommands()
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
void BoardControl::updateOutputInfo()
{
    String msgLine;
    char tempMsgLine[20];

   outSense.sample();

    msgLine = "vA: " + String(outSense.readOutput(Output::VoltageA));
    msgLine.toCharArray(tempMsgLine, 20);
    lcd.setCursor(0,1);
    lcd.print(tempMsgLine);

    msgLine = "vB: " + String(outSense.readOutput(Output::VoltageB));
    msgLine.toCharArray(tempMsgLine, 20);
    lcd.setCursor(10,1);
    lcd.print(tempMsgLine);

    msgLine = "iA: " + String(outSense.readOutput(Output::CurrentA));
    msgLine.toCharArray(tempMsgLine, 20);
    lcd.setCursor(0,2);
    lcd.print(tempMsgLine);

    msgLine = "iB: " + String(outSense.readOutput(Output::CurrentB));
    msgLine.toCharArray(tempMsgLine, 20);
    lcd.setCursor(10,2);
    lcd.print(tempMsgLine);
    mySerial.println("Output info updated");
}


