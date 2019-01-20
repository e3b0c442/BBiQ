#ifndef BUTTON_E3B0C442_H
#define BUTTON_E3B0C442_H

#include <Arduino.h>
#include "event.h"

// 3 buttons:
//
// State            Button3 Button2 Button1
// ALL_UP           O       O       O       = 0
// ONE_DOWN         O       O       X       = 1
// TWO_DOWN         O       X       O       = 2
// ONE_TWO_DOWN     O       X       X       = 3
// THREE_DOWN       X       O       O       = 4
// ONE_THREE_DOWN   X       O       X       = 5
// TWO_THREE_DOWN   X       X       O       = 6
// ALL_DOWN         X       X       X       = 7

enum class ButtonState : uint8_t
{
    ALL_UP = 0,
    ONE_DOWN,
    TWO_DOWN,
    ONE_TWO_DOWN,
    THREE_DOWN,
    ONE_THREE_DOWN,
    TWO_THREE_DOWN,
    ALL_DOWN
};

struct ButtonEvent : Event
{
    ButtonState state : 3;
    ButtonEvent(ButtonState aState);
};

#endif //BUTTON_E3B0C442_H