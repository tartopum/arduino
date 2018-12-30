#include <VirtualWire.h>
#include <multi_channel_relay.h>
#include "Switch.h"

// 433 RF
int RF_RX_PIN = 2;
int BITS_PER_SEC = 2000;
byte MAX_MSG_LEN = 30;

int pinBypass = 8;
int pinMotor = 6;
byte curSpeed = 0;

Multi_Channel_Relay multiRelay1;
Multi_Channel_Relay multiRelay2;

Switch rotor(&multiRelay1, 1, &multiRelay1, 2);
Switch turet(&multiRelay1, 3, &multiRelay1, 4);
Switch loadingArm(&multiRelay2, 1, &multiRelay2, 2);
Switch conveyorBelt(&multiRelay2, 3, &multiRelay2, 4);

void parseMsg(uint8_t *buf, uint8_t nChars, char *key, char *val)
{
    char msg[nChars + 1];
    strncpy(msg, (char *)buf, nChars + 1);
    msg[nChars] = 0;
    strcpy(key, strtok(msg, "="));
    strcpy(val, strtok(NULL, "="));
}

void changeSpeed(int targetSpeed)
{
    // 0-1023 -> 0-255
    targetSpeed = targetSpeed / 4.0;
    int delta = 50;
    if (targetSpeed < curSpeed) delta *= -1;

    while (abs(targetSpeed - curSpeed) > abs(delta)) {
        curSpeed += delta;
        analogWrite(pinMotor, curSpeed);
        delay(50);
    }
    curSpeed = targetSpeed;
    analogWrite(pinMotor, curSpeed);
}

void setup()
{
    Serial.begin(9600);

    multiRelay1.begin(0x11);
    multiRelay2.begin(0x21);

    vw_set_rx_pin(RF_RX_PIN);
    vw_setup(BITS_PER_SEC);
    vw_rx_start();

    changeSpeed(0);
}

void loop()
{
    uint8_t buf[MAX_MSG_LEN];
    uint8_t bufLen = MAX_MSG_LEN;
    if(!vw_get_message(buf, &bufLen)) return;

    char key[10];
    char val[10];
    parseMsg(buf, bufLen, key, val);
    
    if (strcmp(key, "speed") == 0) {
        changeSpeed(atoi(val));
    }
    else {
        switch_state_t state = atoi(val);
        if (strcmp(key, "rotor") == 0) rotor.setState(state);
        if (strcmp(key, "turet") == 0) turet.setState(state);
        if (strcmp(key, "loadingArm") == 0) loadingArm.setState(state);
        if (strcmp(key, "conveyorBelt") == 0) conveyorBelt.setState(state);
    }

    if (rotor.isActive() || turet.isActive() || loadingArm.isActive() || conveyorBelt.isActive()) {
        analogWrite(pinBypass, 255);
    }
    else {
        analogWrite(pinBypass, 0);
    }
}
