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
//    lcd.begin();

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
            fanRunning = shouldFanRkun;
            fanDelay = 50;
            digitalWrite(FAN_CTRL, fanRunning);
        }
    } else {
        fanDelay--;
    }
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

int buttonState = 0;
int buttonFlipDelay = 0;
uint8_t ledStatus = 0;

void BoardControl::checkCommands()
{
//    uint8_t fpResponse[2];
    uint16_t fpResponse;

    // Request button state
    Wire.beginTransmission(0x42);
    int result = Wire.write(0x01);
    if (result == 0) {
        mySerial.println("Error writing to I2C. Result=0");
    }
    //  * Output   0 .. success
    //  *          1 .. length to long for buffer
    //  *          2 .. address send, NACK received
    //  *          3 .. data send, NACK received
    //  *          4 .. other twi error (lost bus arbitration, bus error, ..)
    //  *          5 .. timeout
    result = Wire.endTransmission();
    if (result != 0) {
        mySerial.print("Error writing to I2C. Result=");
        mySerial.println(result);
        delay(2000);
        return;
    }
    Wire.requestFrom(0x42, 2);
    fpResponse = Wire.read();
    fpResponse |= Wire.read() << 8;
    // mySerial.print("Received from frontPanel: 0x");
    // mySerial.println(fpResponse, HEX);

    int MuxPin_GND        = 0b0000000000000000;
    int MuxPin_ABSelect   = 0b0000000000000010;
    int MuxPin_ParSelect  = 0b0000000000000100;
    int MuxPin_SerSelect  = 0b0000000000001000;
    int MuxPin_IndSelect  = 0b0000000001000000;
    int MuxPin_LArrow     = 0b0000000010000000;
    int MuxPin_RArrow     = 0b0000000100000000;
    int MuxPin_ENC_CLK    = 0b0000100000000000;
    int MuxPin_ENC_DT     = 0b0001000000000000;
    int MuxPin_ENC_SWITCH = 0b0010000000000000;
    int MuxPin_CHB_EN     = 0b0100000000000000;
    int MuxPin_CHA_EN     = 0b1000000000000000;

    const int buttonsToCheck[5] = {
        MuxPin_IndSelect,
        MuxPin_ParSelect,
        MuxPin_SerSelect,
        MuxPin_CHA_EN,
        MuxPin_CHB_EN
    };

    // Flip the state according to the pressed buttons
    uint16_t newButtonState = fpResponse;
    if (newButtonState != buttonState) {
        mySerial.print("New button state: ");
        mySerial.println(newButtonState, HEX);

        for (int i = 0; i < 5; ++i) {
            if (!(buttonState & buttonsToCheck[i]) && (newButtonState & buttonsToCheck[i])) {
                // Button 'i' was pressed
                // Change state of led i
                ledStatus = ledStatus ^ (1 << i);
                mySerial.print("led ");
                mySerial.print(i);
                mySerial.print(" is now ");
                mySerial.println((ledStatus & (1 << i))? "ON" : "OFF");
            }
        }
        buttonState = newButtonState;
        Wire.beginTransmission(0x42);
        Wire.write(0x02);
        Wire.write(ledStatus);
        Wire.endTransmission();
        Wire.requestFrom(0x42, 1);
        Wire.read();
    }
}

void BoardControl::updateOutputInfo()
{
    String msgLine;
    char tempMsgLine[20];

//   outSense.sample();

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
//    mySerial.println("Output info updated");
}


