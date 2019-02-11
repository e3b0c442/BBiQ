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
        INT_TEMP,
        UI,
        UART,
        COUNT
    };

    Type type;
    uint32_t ts;

    Event() { type = Type::GENERIC, ts = millis(); };
#ifdef DEBUG
    void prelog(Stream &);
    virtual void log(Stream &);
    virtual ~Event(){};
#endif // DEBUG
};

typedef void (*EventHandler)(Event &e);

void eventSetup();
void registerHandler(Event::Type type, EventHandler handler);
void dispatch(Event *e);