#include <Arduino.h>
#include "button.hpp"
#include "event.hpp"
#include "mode.hpp"
#include "probe.hpp"
#include "ui.hpp"

namespace
{
const uint32_t DISPLAY_SPLASH_TIME = 2000;
const uint32_t DISPLAY_POWERSAVE_TIME = 10000;

bool powerSave = false;
Screen screen = Screen::SPLASH;
Screen nextScreen = Screen::COUNT;

uint32_t lastInteraction = 0;
ButtonState lastButtons = ButtonState::ALL_UP;

uint32_t lastBoot = 0;
Probe *selected = NULL;

void handler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::BUTTON:
    {
        ButtonEvent *be = (ButtonEvent *)e;
        lastInteraction = e->ts;
        ButtonState old = lastButtons;
        lastButtons = be->state;

        if (powerSave)
        {
            powerSave = false;
            dispatch(new UIEvent(screen, powerSave, selected));
            break;
        }

        if (old == ButtonState::ALL_UP)
        {
            switch (be->state)
            {
            case ButtonState::ONE_DOWN:
            {
                if (selected != NULL)
                {
                    Probe *p = selected->next();
                    if (p != NULL)
                    {
                        selected = p;
                        dispatch(new UIEvent(screen, powerSave, selected));
                    }
                }
                break;
            }
            case ButtonState::TWO_DOWN:
            {
                if (selected != NULL)
                {
                    Probe *p = selected->prev();
                    if (p != NULL)
                    {
                        selected = p;
                        dispatch(new UIEvent(screen, powerSave, selected));
                    }
                }
                break;
            }
            default:;
            }
        }
        break;
    }
    case Event::Type::MODE:
    {
        Screen sc = screen;
        ModeEvent *me = (ModeEvent *)e;
        switch (me->mode)
        {
        case RunMode::BOOT:
            lastBoot = e->ts;
            screen = Screen::SPLASH;
            sc = Screen::COUNT;
            dispatch(new UIEvent(Screen::SPLASH, powerSave, selected));
            return;
        case RunMode::PROGRAM:
            sc = Screen::PROGRAM;
            break;
        case RunMode::NORMAL:
            sc = Screen::PROBE;
            break;
        }

        if ((int32_t)e->ts - (int32_t)lastBoot < (int32_t)DISPLAY_SPLASH_TIME)
            nextScreen = sc;
        else
        {
            screen = sc;
            dispatch(new UIEvent(screen, powerSave, selected));
        }
        break;
    }
    case Event::Type::PROBE:
    {
        ProbeEvent *pe = (ProbeEvent *)e;
        if (selected == NULL && pe->probe->connected)
            selected = pe->probe;

        if (selected == pe->probe)
        {
            if (!pe->probe->connected)
                selected = pe->probe->next();

            dispatch(new UIEvent(screen, powerSave, selected));
        }
        break;
    }
    default:;
    }
}
} // namespace

void uiSetup()
{
    registerHandler(Event::Type::BUTTON, &handler);
    registerHandler(Event::Type::MODE, &handler);
    registerHandler(Event::Type::PROBE, &handler);
}

void uiLoop(uint32_t ts)
{
    if (!powerSave && (int32_t)ts - (int32_t)lastInteraction > (int32_t)DISPLAY_POWERSAVE_TIME)
    {
        powerSave = true;
        dispatch(new UIEvent(screen, powerSave, selected));
        return;
    }
    if (screen == Screen::SPLASH && (int32_t)ts - (int32_t)lastBoot > (int32_t)DISPLAY_SPLASH_TIME)
    {
        screen = nextScreen;
        dispatch(new UIEvent(screen, powerSave, selected));
    }
}

#ifdef DEBUG
void UIEvent::log(Stream &s)
{
    const char *screens[] = {
        "SPLASH",
        "PROGRAM",
        "PROBE",
        "COUNT",
    };
    Event::prelog(s);
    s.printf("TYPE: UI; SCREEN: %s; POWERSAVE: %d; PROBE: %d\n", screens[(uint8_t)screen], powerSave, probe->id);
}
#endif // DEBUG