#include <Wire.h>

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);
    Wire.begin();
    Serial.println("Front Panel tester");

}

int buttonState = 0;
int buttonFlipDelay = 0;
uint8_t ledStatus = 0;

void loop()
{
    uint8_t fpResponse[2];

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
    fpResponse[0] = Wire.read();
    fpResponse[1] = Wire.read();
    // Flip the state according to the pressed buttons
    uint16_t newButtonState = fpResponse[1] >> 4;
    if (newButtonState != buttonState) {
        Serial.print("New button state: ");
        Serial.println(newButtonState, HEX);

        for (int i = 0; i < 4; ++i) {
            if (!(buttonState & (1 << i)) && (newButtonState & (1 << i))) {
                // Button 'i' was pressed
                // Change state of led i
                ledStatus = ledStatus ^ (1 << (3-i));
                Serial.print("led ");
                Serial.print(i);
                Serial.print(" is now ");
                Serial.println((ledStatus & (1 << i))? "ON" : "OFF");
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
//    delay(10);
}
