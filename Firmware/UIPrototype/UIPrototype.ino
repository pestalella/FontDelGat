#include "DisplayState.h"

#include <Arduino.h>

uint16_t buttonState;
const int MenuButton = 2;
const int ChAEnButton = 3;
const int ChBEnButton = 4;

DisplayState disp;

void setup()
{
    Serial.begin(115200);
    Serial.println("Front panel test");

    Wire.begin();
    disp.begin();
    buttonState = 0;
    pinMode(MenuButton, INPUT);
    delay(1000);
    disp.updateDisplay();
}

int buttonDelay = 0;

void checkButtons()
{
//    uint8_t fpResponse[2];
    uint16_t fpResponse;

    // Request button state
    Wire.beginTransmission(0x42);
    int result = Wire.write(0x01);
    if (result == 0) {
        Serial.println("Error writing to I2C. Result=0");
    }
    //  * Output   0 .. success
    //  *          1 .. length to long for buffer
    //  *          2 .. address send, NACK received
    //  *          3 .. data send, NACK received
    //  *          4 .. other twi error (lost bus arbitration, bus error, ..)
    //  *          5 .. timeout
    result = Wire.endTransmission();
    if (result != 0) {
        Serial.print("Error writing to I2C. Result=");
        Serial.println(result);
        delay(2000);
        return;
    }
    Wire.requestFrom(0x42, 2);
    fpResponse = Wire.read();
    fpResponse |= Wire.read() << 8;
    // Serial.print("Received from frontPanel: 0x");
    // Serial.println(fpResponse, HEX);

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

    // Flip the state according to the pressed buttons
    uint16_t newButtonState = fpResponse;
    if (newButtonState != buttonState) {
        if (buttonDelay == 0) {
            // Serial.print("Button state: 0x");
            // Serial.println(newButtonState, HEX);
            if ((buttonState & MuxPin_ABSelect) == 0 && (newButtonState & MuxPin_ABSelect) != 0) {
                Serial.println("Menu pressed");
                disp.menuPressed();
            }
            if ((buttonState & MuxPin_CHA_EN) == 0 && (newButtonState & MuxPin_CHA_EN) != 0) {
                Serial.println("ChAEn pressed");
                disp.chAEnablePressed();
            }
            if ((buttonState & MuxPin_CHB_EN) == 0 && (newButtonState & MuxPin_CHB_EN) != 0) {
                Serial.println("ChBEn pressed");
                disp.chBEnablePressed();
            }
            if ((buttonState & MuxPin_IndSelect) == 0 && (newButtonState & MuxPin_IndSelect) != 0) {
                Serial.println("Independent output pressed");
                disp.independentOutputPressed();
            }
            if ((buttonState & MuxPin_ParSelect) == 0 && (newButtonState & MuxPin_ParSelect) != 0) {
                Serial.println("Parallel output pressed");
                disp.parallelOutputPressed();
            }
            if ((buttonState & MuxPin_SerSelect) == 0 && (newButtonState & MuxPin_SerSelect) != 0) {
                Serial.println("Series output pressed");
                disp.seriesOutputPressed();
            }

            disp.updateDisplay();
            buttonState = newButtonState;
            buttonDelay = 10;
        } else {
            buttonDelay--;
        }
        // buttonState = newButtonState;
        // Wire.beginTransmission(0x42);
        // Wire.write(0x02);
        // Wire.write(ledStatus);
        // Wire.endTransmission();
        // Wire.requestFrom(0x42, 1);
        // Wire.read();
    }
}





void loop()
{
    checkButtons();
}
