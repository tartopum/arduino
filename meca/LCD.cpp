#include <rgb_lcd.h>
#include "LCD.h"

rgb_lcd lcd;

void initLCD()
{
    lcd.begin(20, 2);
    lcd.setRGB(255, 0, 0);
    lcd.print("Init...");
}

void clearLCD()
{
    lcd.clear();
    lcd.home();
}
