#pragma once

#include <Arduino.h>
#include "event.hpp"

// 3 buttons:
//
// State            Button3 Button2 Button1
// ALL_DOWN         L       L       L       = 0
// TWO_THREE_DOWN   L       L       H       = 1
// ONE_THREE_DOWN   L       H       L       = 2
// THREE_DOWN       L       H       H       = 3
// ONE_TWO_DOWN     H       L       L       = 4
// TWO_DOWN         H       L       H       = 5
// ONE_DOWN         H       H       L       = 6
// ALL_UP           H       H       H       = 7

enum class ButtonState : uint8_t
{
    ALL_DOWN = 0,
    TWO_THREE_DOWN,
    ONE_THREE_DOWN,
    THREE_DOWN,
    ONE_TWO_DOWN,
    TWO_DOWN,
    ONE_DOWN,
    ALL_UP
};

struct ButtonEvent : Event
{
    ButtonState state;
    ButtonEvent(ButtonState aState) { type = Event::Type::BUTTON, state = aState; };

#ifdef DEBUG
    void log(Stream &s);
#endif // DEBUG
};

void buttonSetup();
void buttonLoop(uint32_t ts);