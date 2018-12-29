#include <VirtualWire.h>

int MOTOR_PIN = 6;
int RF_RX_PIN = 2;
int BITS_PER_SEC = 2000;
byte MAX_MSG_LEN = 10;

int curSpeed = 0;

int parseSpeed(uint8_t *buf, uint8_t nChars)
{
    char msg[nChars + 1];
    strncpy(msg, (char *)buf, nChars + 1);
    msg[nChars] = 0;
    return atoi(msg) / 4.0;
}

void changeSpeed(int targetSpeed)
{
    Serial.println(targetSpeed);
}

void setup()
{
    Serial.begin(9600);
    pinMode(MOTOR_PIN, OUTPUT);
    vw_set_rx_pin(RF_RX_PIN);
    vw_setup(BITS_PER_SEC);
    vw_rx_start();
    changeSpeed(0);
}

void loop()
{
    uint8_t buf[MAX_MSG_LEN];
    uint8_t bufLen = MAX_MSG_LEN;
    if(vw_get_message(buf, &bufLen)) {
        changeSpeed(parseSpeed(buf, bufLen));
    }
}
