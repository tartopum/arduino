#include "Pailleuse.h"

Pailleuse::Pailleuse(int pinTransmitter, int pinSpeed, int pinForward, int pinBackward)
{
    _pinTransmitter = pinTransmitter;
    _pinSpeed = pinSpeed;
    _pinForward = pinForward;
    _pinBackward = pinBackward;

    pinMode(_pinForward, INPUT_PULLUP);
    pinMode(_pinBackward, INPUT_PULLUP);
}

bool isOn(int pinBtn)
{
    return (digitalRead(pinBtn) == LOW);
}

void Pailleuse::setup()
{
    vw_set_tx_pin(_pinTransmitter);
    vw_setup(_bitsPerSec);
    sendSpeed();
}

void Pailleuse::loop()
{
    computeSpeed();
    if (canSend()) {
        sendSpeed();
    }
}

void Pailleuse::computeSpeed()
{
    _speed = analogRead(_pinSpeed);
    if (isOn(_pinBackward)) {
        _speedToSend = -1 * _speed;
    }
    else if (isOn(_pinForward)) {
        _speedToSend = _speed;
    }
    else {
        _speedToSend = 0;
    }
}

bool Pailleuse::canSend()
{
    return ( 
        (abs(_speedToSend - _lastSentSpeed) > speedAccuracy) &&
        ((millis() - _lastMsgTime) > delayBetweenMessages)
    );
}

void Pailleuse::sendSpeed()
{
    _lastMsgTime = millis();
    _lastSentSpeed = _speedToSend;
    char msg[10];
    String(_speedToSend).toCharArray(msg, 10);
    vw_send((uint8_t *)msg, strlen(msg));
}
