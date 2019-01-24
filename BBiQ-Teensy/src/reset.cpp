#include <Arduino.h>
#include "button.hpp"
#include "event.hpp"
#include "mode.hpp"
#include "pins.hpp"
#include "reset.hpp"

namespace
{
const ButtonState RESET_STATE_NORMAL = ButtonState::TWO_THREE_DOWN;
const ButtonState RESET_STATE_PROGRAM = ButtonState::ONE_THREE_DOWN;
const uint32_t RESET_DELAY = 1000;

ButtonState lastState;
uint32_t lastStateChange;
bool triggered;

void handler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::BUTTON:
    {
        ButtonEvent *be = (ButtonEvent *)e;
        if (triggered)
        {
            triggered = false;
            switch (lastState)
            {
            case RESET_STATE_NORMAL:
                reset(RunMode::NORMAL);
                break;
            case RESET_STATE_PROGRAM:
                reset(RunMode::PROGRAM);
                break;
            default:;
            }
        }
        if (lastState != be->state)
        {
            lastState = be->state;
            lastStateChange = be->ts;
        }
        break;
    }
    default:;
    }
}
}; // namespace

void reset(RunMode mode)
{
    Serial3.end();
    pinMode((uint8_t)Pin::ESP_IO2, OUTPUT);
    digitalWrite((uint8_t)Pin::ESP_IO2, HIGH);

    switch (mode)
    {
    case RunMode::NORMAL:
        digitalWrite((uint8_t)Pin::ESP_IO0, HIGH);
        break;
    case RunMode::PROGRAM:
        digitalWrite((uint8_t)Pin::ESP_IO0, LOW);
        break;
    default:;
    }

    digitalWrite((uint8_t)Pin::ESP_RST, LOW);
    delay(50);
    digitalWrite((uint8_t)Pin::ESP_RST, HIGH);

    dispatch(new ResetEvent(mode));
}

void resetSetup()
{
    registerHandler(Event::Type::BUTTON, &handler);
    registerHandler(Event::Type::MODE, &handler);
    pinMode((uint8_t)Pin::ESP_IO0, OUTPUT);
    pinMode((uint8_t)Pin::ESP_IO2, OUTPUT);
    pinMode((uint8_t)Pin::ESP_RST, OUTPUT);
    digitalWrite((uint8_t)Pin::ESP_RST, LOW);
}

void resetLoop(uint32_t *ts)
{
    if (!triggered &&
        (lastState == RESET_STATE_NORMAL || lastState == RESET_STATE_PROGRAM) &&
        int32_t(*ts) - int32_t(RESET_DELAY) > int32_t(lastStateChange))
    {
        triggered = true;
    }
}