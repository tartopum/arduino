#ifndef Pailleuse_h
#define Pailleuse_h

#include <Arduino.h>
#include <VirtualWire.h>
#include "Switch.h"

class Pailleuse
{
    public:
        Pailleuse(
            byte pinTransmitter,
            byte pinSpeed,
            byte pinsRotor[2],
            byte pinsTuret[2],
            byte pinsLoadingArm[2],
            byte pinsConveyorBelt[2]
        );
        void setup();
        void loop();

        byte speedMinDelta = 3; // To ignore potentiometer noise
        byte delayBetweenMessages = 20; // ms

    private:
        byte _pinTransmitter;
        byte _pinSpeed;
        int _bitsPerSec = 2000;

        Switch _rotor;
        Switch _turet;
        Switch _loadingArm;
        Switch _conveyorBelt;

        unsigned int _speed = 0; // 0 <= _speed < 1024
        unsigned int _speedToSend = 0;
        unsigned int _lastSpeedSent = 0;

        void _sendAll(bool noDuplicate = true);
        void _computeSpeed();
        void _sendMsg(char *msg);
        void _sendSpeed(bool noDuplicate = true);
        void _sendSwitchState(Switch *sw, bool noDuplicate = true);
};

#endif
