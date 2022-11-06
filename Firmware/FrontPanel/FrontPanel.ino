#include "FrontPanel.h"

FrontPanel fp;

void setup()
{
    fp.begin();
}

void loop()
{
    fp.update();
}
