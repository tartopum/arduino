#include "Pailleuse.h"

Pailleuse::Pailleuse(
    byte pinTransmitter,
    byte pinSpeed,
    byte pinsRotor[2],
    byte pinsTuret[2],
    byte pinsLoadingArm[2],
    byte pinsConveyorBelt[2]
) :
    _rotor("rotor", pinsRotor[0], pinsRotor[1]),
    _turet("turet", pinsTuret[0], pinsTuret[1]),
    _loadingArm("loadingArm", pinsLoadingArm[0], pinsLoadingArm[1]),
    _conveyorBelt("conveyorBelt", pinsConveyorBelt[0], pinsConveyorBelt[1])
{
    _pinTransmitter = pinTransmitter;
    _pinSpeed = pinSpeed;
}

void Pailleuse::setup()
{
    vw_set_tx_pin(_pinTransmitter);
    vw_setup(_bitsPerSec);
    _sendAll(false);
}

void Pailleuse::loop()
{
    _sendAll(true);
}

void Pailleuse::_sendAll(bool noDuplicate)
{
    _computeSpeed();

    _sendSpeed(noDuplicate);
    delay(delayBetweenMessages);

    _sendSwitchState(&_rotor, noDuplicate);
    delay(delayBetweenMessages);
    
    _sendSwitchState(&_turet, noDuplicate);
    delay(delayBetweenMessages);
    
    _sendSwitchState(&_loadingArm, noDuplicate);
    delay(delayBetweenMessages);
    
    _sendSwitchState(&_conveyorBelt, noDuplicate);
    delay(delayBetweenMessages);
}

void Pailleuse::_computeSpeed()
{
    unsigned int newSpeed = 1023 - analogRead(_pinSpeed);
    if (abs(newSpeed - _speed) > speedMinDelta) {
        _speed = newSpeed;
    }

    if (_rotor.readState() == RIGHT) {
        _speedToSend = _speed;
    }
    else {
        _speedToSend = 0;
    }
}

void Pailleuse::_sendSwitchState(Switch *sw, bool noDuplicate)
{
    if (!sw->hasChanged() && noDuplicate) return;
    char msg[30];
    sw->toMsg(msg);
    _sendMsg(msg);
}

void Pailleuse::_sendSpeed(bool noDuplicate)
{
    if (_lastSpeedSent == _speedToSend && noDuplicate) return;
    _lastSpeedSent = _speedToSend;
    char msg[15];
    sprintf(msg, "speed=%i", _speedToSend);
    _sendMsg(msg);
}

void Pailleuse::_sendMsg(char *msg)
{
    Serial.println(msg);
    vw_send((uint8_t *)msg, strlen(msg));
    vw_wait_tx();
}
