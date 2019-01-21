#include <Arduino.h>
#include "button.h"
#include "event.h"
#include "pins.h"
#include "reset.h"
#include "mode.h"

namespace
{
const uint32_t RESET_DELAY = 1000;
const ButtonState RESET_STATE_NORMAL = ButtonState::ONE_TWO_DOWN;
const ButtonState RESET_STATE_PROGRAM = ButtonState::TWO_THREE_DOWN;

ButtonState lastState;
uint32_t lastStateChange = 0;
bool triggered;

void eventHandler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::BUTTON:
        ButtonEvent *be = (ButtonEvent *)e;
        if (lastState != be->state)
        {
            lastState = be->state;
            lastStateChange = be->ts;
        }
        break;
    case Event::Type::MODE:
        ModeEvent *me = (ModeEvent *)e;
        if (me->mode != RunMode::BOOT)
        {
            triggered = false;
        }
        break;
    }
}

void reset(uint32_t ts)
{
    Serial2.end();
    pinMode((uint8_t)Pin::ESP_IO2, OUTPUT);
    digitalWrite((uint8_t)Pin::ESP_IO2, HIGH);
    RunMode mode;

    switch (lastState)
    {
    case RESET_STATE_NORMAL:
        digitalWrite((uint8_t)Pin::ESP_IO0, HIGH);
        mode = RunMode::NORMAL;
        break;
    case RESET_STATE_PROGRAM:
        digitalWrite((uint8_t)Pin::ESP_IO0, LOW);
        mode = RunMode::PROGRAM;
        break;
    }

    digitalWrite((uint8_t)Pin::ESP_RST, LOW);
    delay(50);
    digitalWrite((uint8_t)Pin::ESP_RST, HIGH);
    dispatch(new ResetEvent(mode));
}
}; // namespace

void resetSetup()
{
    registerHandler(Event::Type::BUTTON, &eventHandler);
    registerHandler(Event::Type::MODE, &eventHandler);
}
void resetLoop(uint32_t ts)
{
    if (!triggered &&
        (lastState == RESET_STATE_NORMAL || lastState == RESET_STATE_PROGRAM) &&
        int32_t(ts) - int32_t(RESET_DELAY) > int32_t(lastStateChange))
    {
        reset(ts);
    }
}
