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
        PROBE,
        COUNT
    };

    Type type;
    uint32_t ts;

    Event() { type = Type::GENERIC, ts = millis(); };
};

typedef void (*EventHandler)(Event *e);

void eventSetup();
void registerHandler(Event::Type type, EventHandler handler);
void dispatch(Event *e);