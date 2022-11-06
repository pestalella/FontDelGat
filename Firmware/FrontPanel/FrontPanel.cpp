#include "FrontPanel.h"
#include <Arduino.h>
#include <Wire.h>
#include <wiring_digital.h>

FrontPanel::FrontPanel()
    //Serial(PA3, PA2)
{

}

int receivedCommand = 0;
int receivedSize = 0;
char receiveBuffer[10];
uint16_t buttonState = 0;
int ledStatusMask = 0;

int lastStateCLK = 0;

void receiveEvent(int msgSize)
{
    if (msgSize > 10) {
        Serial.println("Message too large received :(");
        return;
    }
    receivedSize = 0;
    while (Wire.available() > 0) {
        receiveBuffer[receivedSize] = Wire.read();
        receivedSize++;
    }
    if (receivedSize != msgSize) {
        Serial.print("[ERROR] Expected msgSize = ");
        Serial.print(msgSize);
        Serial.print("   Actual msgSize = ");
        Serial.println(receivedSize);
        return;
    }
}


uint16_t lastStateSent = 0;
void requestEvent()
{
    uint8_t sendBuf[2];
    switch (receiveBuffer[0]) {
    case 0x01:  // Read keyboard status
        sendBuf[0] = buttonState & 0xFF;
        sendBuf[1] = buttonState >> 8;
        if (buttonState != lastStateSent) {
            Serial.print("[CMD] Read keyboard status. Sending ");
            Serial.println(buttonState, HEX);
            lastStateSent = buttonState;
        }
        Wire.write(sendBuf, 2);
        break;
    case 0x02:  // Set LED status
        ledStatusMask = receiveBuffer[1];
        Wire.write(0x00);  // Send OK
        break;
    default:
        Serial.print("Unknown command received: [");
        for (int i = 0; i < receivedSize; ++i) {
            Serial.print("0x");
            Serial.print(receiveBuffer[i], HEX);
            Serial.print(" ");
        }
        Serial.println("]");
    }
}

void FrontPanel::begin()
{
    Serial.begin(115200);
    delay(100);
    Serial.println("Front Panel setup start");

    Wire.setSCL(I2C_SCL);
    Wire.setSDA(I2C_SDA);
    Wire.begin(0x42);
    Wire.onRequest(requestEvent);
    Wire.onReceive(receiveEvent);

    pinMode(INDEP_CHANNEL_EN, OUTPUT);
    pinMode(PAR_CHANNEL_EN, OUTPUT);
    pinMode(SERIES_CHANNEL_EN, OUTPUT);
    pinMode(CHANNEL_A_EN, OUTPUT);
    pinMode(CHANNEL_B_EN, OUTPUT);

    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(N_SCAN_EN, OUTPUT);
    pinMode(COM, INPUT_PULLDOWN);
    // Disable the multiplexer
    digitalWrite(N_SCAN_EN, 1);


//	lastStateCLK = 0;


    Serial.println("Front Panel setup end");
}


void FrontPanel::updateLeds()
{
    digitalWrite( INDEP_CHANNEL_EN, (ledStatusMask & 0x01) != 0);
    digitalWrite(   PAR_CHANNEL_EN, (ledStatusMask & 0x02) != 0);
    digitalWrite(SERIES_CHANNEL_EN, (ledStatusMask & 0x04) != 0);
    digitalWrite(     CHANNEL_A_EN, (ledStatusMask & 0x08) != 0);
    digitalWrite(     CHANNEL_B_EN, (ledStatusMask & 0x10) != 0);
}

uint16_t FrontPanel::readButtonState(int buttonNum)
{
    // Disable while changing the address
    digitalWrite(N_SCAN_EN, 1);
    // Set up address to read
    digitalWrite(S0, buttonNum & 0x01);
    digitalWrite(S1, (buttonNum & 0x02) >> 1);
    digitalWrite(S2, (buttonNum & 0x04) >> 2);
    digitalWrite(S3, (buttonNum & 0x08) >> 3);
    // Enable multiplexer
    digitalWrite(N_SCAN_EN, 0);
    int state = digitalRead(COM);
    return (state == HIGH)? 1 : 0;
}

uint16_t FrontPanel::scanKeys()
{
    uint16_t newButtonState = 0;
    for (int i = 0; i < 16; ++i) {
        newButtonState = newButtonState | (readButtonState(i) << i);
    }
    return newButtonState;
}

/***
 * MUX inputs
 * ----------
 *  0: GND
 *  1: ABSelect
 *  2: ParSelect
 *  3: SerSelect
 *  4: GND
 *  5: GND
 *  6: IndSelect
 *  7: LArrow
 *  8: RArrow
 *  9: GND
 * 10: GND
 * 11: ENC_CLK
 * 12: ENC_DT
 * 13: ENC_SWITCH
 * 14: CHB_EN
 * 15: CHA_EN
 ***/
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

int onLed = 0;
int ledId[5] = {1,4,2,8,16};

void FrontPanel::update()
{
    const int buttonsToCheck[5] = {
        MuxPin_IndSelect,
        MuxPin_ParSelect,
        MuxPin_SerSelect,
        MuxPin_CHA_EN,
        MuxPin_CHB_EN
    };
    int newButtonState = scanKeys();
    if (newButtonState != buttonState) {
        Serial.print("New button state: ");
        Serial.println(newButtonState, HEX);

    //     for (int i = 0; i < 5; ++i) {
    //         if (!(buttonState & buttonsToCheck[i]) && (newButtonState & buttonsToCheck[i])) {
    //             // Button 'buttonIdx' was pressed
    //             // Change state of led i
    //             ledStatusMask = ledStatusMask ^ (1 << i);
    //         }
    //     }
    //    buttonState = newButtonState;
    }
	buttonState = newButtonState;
    // Read the current state of CLK
	// int currentStateCLK = (buttonState & MuxPin_ENC_CLK) != 0;
	// int currentStateDT = (buttonState & MuxPin_ENC_DT) != 0;

	// // If last and current state of CLK are different, then pulse occurred
	// // React to only 1 state change to avoid double count
	// if (currentStateCLK != lastStateCLK  && currentStateCLK == 1){
	// 	// If the DT state is different than the CLK state then
	// 	// the encoder is rotating CCW so decrement
	// 	if (currentStateDT != currentStateCLK) {
    //         onLed = (onLed + 1)%5;
	// 	} else {
	// 		// Encoder is rotating CW so increment
    //         onLed = (onLed + 4)%5;
	// 	}
	// }

	// // Remember last CLK state
	// lastStateCLK = currentStateCLK;

	// // Read the button state
	// int btnState = digitalRead(SW);

	// //If we detect LOW signal, button is pressed
	// if (btnState == LOW) {
	// 	//if 50ms have passed since last LOW pulse, it means that the
	// 	//button has been pressed, released and pressed again
	// 	if (millis() - lastButtonPress > 50) {
	// 		Serial.println("Button pressed!");
	// 	}

	// 	// Remember last button press event
	// 	lastButtonPress = millis();
	// }

//    onLed = (onLed + 1)%5;
//    onLed = 4;
//    ledStatusMask = ledId[onLed];
//    Serial.println(ledStatusMask);

    updateLeds();
}
