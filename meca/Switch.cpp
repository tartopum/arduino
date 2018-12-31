#include "Switch.h"

Switch::Switch(const char *name, byte pinLeft, byte pinRight)
{
    _pinLeft = pinLeft;
    _pinRight = pinRight;

    strcpy(_name, name);
    pinMode(_pinLeft, INPUT_PULLUP);
    pinMode(_pinRight, INPUT_PULLUP);
}

bool isOn(int pinBtn)
{
    return (digitalRead(pinBtn) == LOW);
}

switch_state_t Switch::getState()
{
    return _state;
}

bool Switch::hasChanged()
{
    if (millis() - _lastChangeTime < debounceDelay) return false;
    _lastChangeTime = millis();

    switch_state_t state = CENTER;
    if (isOn(_pinLeft)) state = LEFT;
    else if (isOn(_pinRight)) state = RIGHT;

    bool changed = (state != _state);
    _state = state;
    return changed;
}

void Switch::toMsg(char *dest)
{
    sprintf(dest, "%s=%i", _name, _state);
}
