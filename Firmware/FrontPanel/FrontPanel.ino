#include <Arduino.h>
#include <HardwareSerial.h>
#include <Wire.h>


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

int receivedCommand = 0;
int receivedSize = 0;
char receiveBuffer[10];
uint16_t buttonState = 0;
int ledStatus = 0;

void receiveEvent(int msgSize)
{
    // Serial.println("receiveEvent()");
    if (msgSize > 10) {
        Serial.println("Message too large received :(");
        return;
    }
    receivedSize = 0;
    while (Wire.available() > 0) {
        receiveBuffer[receivedSize] = Wire.read();
        receivedSize++;
    }
    // Serial.print(" MSG received msgSize = ");
    // Serial.println(receivedSize);
    if (receivedSize != msgSize) {
        Serial.print("[ERROR] Expected msgSize = ");
        Serial.print(msgSize);
        Serial.print("   Actual msgSize = ");
        Serial.println(receivedSize);
        return;
    }
}

void updateLeds()
{
    digitalWrite( INDEP_CHANNEL_EN, (ledStatus & 0x01) != 0);
    digitalWrite(   PAR_CHANNEL_EN, (ledStatus & 0x02) != 0);
    digitalWrite(SERIES_CHANNEL_EN, (ledStatus & 0x04) != 0);
    digitalWrite(     CHANNEL_A_EN, (ledStatus & 0x08) != 0);
}

uint16_t lastStateSent = 0;
void requestEvent()
{
    // Serial.println("requestEvent()");
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
        ledStatus = receiveBuffer[1];
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

uint16_t readButtonState(int buttonNum)
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

void scanKeys()
{
    uint16_t newButtonState = 0;
    for (int i = 0; i < 16; ++i) {
        newButtonState = newButtonState | (readButtonState(i) << i);
    }
    if (newButtonState != buttonState) {
        Serial.println(buttonState, HEX);
        buttonState = newButtonState;
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Front Panel setup start");

    Wire.setSCL(I2C_SCL);
    Wire.setSDA(I2C_SDA);
    Wire.begin(0x42);
//    Wire.begin();
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
    Serial.println("Front Panel setup end");
}

int wait = 500;
int counter = 0;

void loop()
{
    scanKeys();
    updateLeds();

    // digitalWrite(INDEP_CHANNEL_EN, HIGH);
    // delay(wait);
    // digitalWrite(INDEP_CHANNEL_EN, LOW);

    // digitalWrite(PAR_CHANNEL_EN, HIGH);
    // delay(wait);
    // digitalWrite(PAR_CHANNEL_EN, LOW);

    // digitalWrite(SERIES_CHANNEL_EN, HIGH);
    // delay(wait);
    // digitalWrite(SERIES_CHANNEL_EN, LOW);

    // digitalWrite(CHANNEL_A_EN, HIGH);
    // delay(wait);
    // digitalWrite(CHANNEL_A_EN, LOW);

    // Serial.print(counter++);
    // Serial.println(" Hi!");
}
