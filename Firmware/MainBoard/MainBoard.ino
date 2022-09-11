#include "BoardControl.h"

BoardControl bc;

void setup()
{
    bc.begin();
}

int wait = 100;
int count = 50;

void loop()
{
    bc.checkFan();
    bc.checkCommands();

    if (--count == 0) {
        bc.updateOutputInfo();
        count = 50;
    }
    delay(wait);
}
