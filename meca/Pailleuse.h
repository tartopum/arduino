#ifndef Pailleuse_h
#define Pailleuse_h

#include <Arduino.h>
#include <VirtualWire.h>

class Pailleuse
{
    public:
        Pailleuse(int pinTransmitter, int pinSpeed, int pinForward, int pinBackward);
        void setup();
        void loop();
        void sendSpeed();
        void computeSpeed();
        bool canSend();
        byte speedAccuracy = 5; // To ignore potentiometer noise
        byte delayBetweenMessages = 100; // ms

    private:
        int _pinTransmitter;
        int _pinSpeed;
        int _pinForward;
        int _pinBackward;
        int _bitsPerSec = 2000;
        unsigned int _speed = 0; // 0 <= _speed < 1024
        int _speedToSend = 0;
        int _lastSentSpeed = 0;
        unsigned long _lastMsgTime = 0; // To ignore button noise
};

#endif
