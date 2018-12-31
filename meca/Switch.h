#ifndef Switch_h
#define Switch_h

#include <Arduino.h>

typedef enum {
    LEFT = -1,
    CENTER = 0,
    RIGHT = 1
} switch_state_t;

class Switch
{
    public:
        Switch(const char *name, byte pinLeft, byte pinRight);
        bool hasChanged();
        switch_state_t getState();
        void toMsg(char *dest);
        unsigned long lastTimeSent = 0;

        byte debounceDelay = 50;

    private:
        byte _pinLeft;
        byte _pinRight;
        switch_state_t _state = CENTER;
        char _name[15];
        unsigned long _lastChangeTime = 0;
};

#endif
