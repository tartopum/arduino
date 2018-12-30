#include "Pailleuse.h"
#include "LCD.h"
//#include "WiFi.h"

byte pinsRotor[2] = {3, 4};
byte pinsTuret[2] = {5, 6};
byte pinsLoadingArm[2] = {7, 8};
byte pinsConveyorBelt[2] = {9, 10};
Pailleuse pailleuse(2, A1, pinsRotor, pinsTuret, pinsLoadingArm, pinsConveyorBelt);

void setup()
{
    Serial.begin(9600); // TODO
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
