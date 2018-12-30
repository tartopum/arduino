#ifndef Switch_h
#define Switch_h

#include <Arduino.h>
#include <multi_channel_relay.h>

typedef enum {
    LEFT = -1,
    CENTER = 0,
    RIGHT = 1
} switch_state_t;

class Switch
{
    public:
        Switch(
            Multi_Channel_Relay *multiRelayLeft,
            byte idRelayLeft,
            Multi_Channel_Relay *multiRelayRight,
            byte idRelayRight
        );
        void setup();
        void setState(switch_state_t);
        bool isActive();

    private:
        Multi_Channel_Relay *_multiRelayLeft = NULL;
        byte _idRelayLeft = 0;
        Multi_Channel_Relay *_multiRelayRight = NULL;
        byte _idRelayRight = 0;
        switch_state_t _state = CENTER;

        void _cmdRelay(switch_state_t, bool);
        void _cmdRelays();
        bool _hasRelay(switch_state_t);
        Multi_Channel_Relay *_stateToMultiRelay(switch_state_t);
        byte _stateToRelayId(switch_state_t);
};

#endif
