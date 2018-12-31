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
        // The receiver needs time to treat the message
        byte delayBetweenMessages = 200; // ms
        unsigned long repeatDelay = 1000;

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
        unsigned long _lastTimeSpeedSent = 0;

        bool _canSend(bool, bool, unsigned long);
        void _sendAll(bool force = false);
        void _computeSpeed();
        void _sendMsg(char *msg);
        void _sendSpeed(bool force = false);
        void _sendSwitchState(Switch *sw, bool force = false);
};

#endif
