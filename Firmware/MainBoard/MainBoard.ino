#include "BoardControl.h"

void setup()
{
    initPins();
}

int wait = 100;

void loop()
{
    checkFan();
    checkCommands();

    delay(wait);
}
