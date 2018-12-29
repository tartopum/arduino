#include "Pailleuse.h"
#include "LCD.h"
//#include "WiFi.h"

Pailleuse pailleuse(2, A1, 3, 4);

void setup()
{
    initLCD();
    //setupWiFi();
    clearLCD();
    pailleuse.setup();
}

void loop()
{
    //handleWiFi();
    pailleuse.loop();
}
