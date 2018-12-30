#include "Switch.h"

Switch::Switch(
    Multi_Channel_Relay *multiRelayLeft,
    byte idRelayLeft,
    Multi_Channel_Relay *multiRelayRight,
    byte idRelayRight
)
{
    _multiRelayLeft = multiRelayLeft;
    _idRelayLeft = idRelayLeft;
    _multiRelayRight = multiRelayRight;
    _idRelayRight = idRelayRight;
    _cmdRelays();
}

bool Switch::isActive()
{
    return _state != CENTER;
}

Multi_Channel_Relay *Switch::_stateToMultiRelay(switch_state_t state)
{
    if (state == LEFT) return _multiRelayLeft;
    if (state == RIGHT) return _multiRelayRight;
    return NULL;
}

byte Switch::_stateToRelayId(switch_state_t state)
{
    if (state == LEFT) return _idRelayLeft;
    if (state == RIGHT) return _idRelayRight;
    return 0;
}

bool Switch::_hasRelay(switch_state_t state)
{
    Multi_Channel_Relay *multiRelay = _stateToMultiRelay(state);
    byte id = _stateToRelayId(state);
    return multiRelay != NULL && 1 <= id && id <= 4;
}

void Switch::setState(switch_state_t state)
{
    if (state == _state) return;
    _state = state;
    _cmdRelays();
}

void Switch::_cmdRelay(switch_state_t state, bool on)
{
    if (!_hasRelay(state)) return;

    Multi_Channel_Relay *multiRelay = _stateToMultiRelay(state);
    byte id = _stateToRelayId(state);
    on ? multiRelay->turn_on_channel(id) : multiRelay->turn_off_channel(id);
}

void Switch::_cmdRelays()
{
    _cmdRelay(LEFT, _state == LEFT);
    _cmdRelay(RIGHT, _state == RIGHT);
}
