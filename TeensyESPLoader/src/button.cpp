#include <Arduino.h>
#include "button.h"
#include "event.h"
#include "pins.h"

namespace
{
const uint32_t BUTTON_DEBOUNCE_DELAY = 50;

struct Button;

const Button::ID BUTTON_IDS[] = {
    Button::BUTTON_0,
    Button::BUTTON_1,
    Button::BUTTON_2,
};

const Pin BUTTON_PINS[] = {
    Pin::BUTTON_1,
    Pin::BUTTON_2,
    Pin::BUTTON_3,
};

struct Button
{
    enum ID
    {
        BUTTON_0,
        BUTTON_1,
        BUTTON_2,
        COUNT
    };

    ID id;
    Pin pin;
    bool state;
    bool reading;
    uint32_t lastChange;

    void read(uint32_t millis);
};

void Button::read(uint32_t ts)
{
    bool aReading = digitalRead((uint8_t)pin);
    if (aReading != reading)
    {
        lastChange = ts;
        reading = aReading;
    }
    if (reading != state)
        if (int32_t(ts) - int32_t(lastChange) > int32_t(BUTTON_DEBOUNCE_DELAY))
            state = reading;
}

Button buttons[Button::COUNT];
ButtonState state = ButtonState::ALL_UP;

ButtonState getState(Button *buttons, uint8_t len)
{
    ButtonState newState = ButtonState(0);
    for (uint8_t i = 0; i < len; i++)
    {
        newState = (ButtonState)((uint8_t)newState | ((uint8_t)buttons[i].state << i));
    }
    return newState;
}

void readButtons(Button *buttons, uint8_t len, unsigned long ts)
{
    for (uint8_t i = 0; i < len; i++)
    {
        buttons[i].read(ts);
    }
    ButtonState newState = getState(buttons, len);
    if (newState != state)
    {
        state = newState;
        dispatch(new ButtonEvent(state));
    }
}
} // namespace

void buttonReset()
{
    uint32_t now = millis();
    for (uint8_t i = 0; i < Button::COUNT; i++)
    {
        buttons[i].reading = digitalRead((uint8_t)buttons[i].pin);
        buttons[i].state = buttons[i].reading;
        buttons[i].lastChange = now;
    }
    state = getState(buttons, Button::COUNT);
}

void buttonSetup()
{
    for (uint8_t i = 0; i < Button::COUNT; i++)
    {
        pinMode((uint8_t)BUTTON_PINS[i], INPUT_PULLUP);
        buttons[i].id = BUTTON_IDS[i];
        buttons[i].pin = BUTTON_PINS[i];
    }
    buttonReset();
}

void buttonLoop(uint32_t ts)
{
    readButtons(buttons, Button::COUNT, ts);
}