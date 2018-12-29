#include "Pailleuse.h"
#include "LCD.h"
//#include "WiFi.h"

Pailleuse pailleuse(2, A1, 6, 7);

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
