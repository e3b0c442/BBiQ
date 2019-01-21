#pragma once

#include <Arduino.h>

struct Event
{
    enum class Type : uint8_t
    {
        GENERIC,
        RESET,
        MODE,
        BUTTON,
        //SERIAL_RX_EVENT,
        //BUTTON_DOWN_EVENT,
        //BUTTON_UP_EVENT,
        //PROBE_CONNECT_EVENT,
        //PROBE_DISCONNECT_EVENT,
        //PROBE_CHANGE_EVENT,
        //PROBE_ALARM_EVENT,
        //LOCAL_INPUT_EVENT,
        //UI_CHANGE_EVENT,
        //UI_CHANGE_SCREEN_EVENT,
        COUNT
    };

    Type type;
    unsigned long ts;

    Event();
};

typedef void (*EventHandler)(Event *);

void eventSetup();
void registerHandler(Event::Type type, EventHandler handler);
void dispatch(Event *e);
