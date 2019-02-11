#pragma once

#include <Arduino.h>
#include "event.hpp"
#include "pins.hpp"

struct Probe
{
    enum class ID : uint8_t
    {
        PROBE_0,
        PROBE_1,
        PROBE_2,
        //PROBE_3,
        COUNT
    };

    ID id;
    Pin pin;
    bool connected;
    float temperature;
    int lowAlarm;
    int highAlarm;
    bool arm;
    char name[16];

    void read();
    Probe *next();
    Probe *prev();
};

struct ProbeEvent : Event
{
    enum class Action : uint8_t
    {
        CONNECT,
        DISCONNECT,
        TEMP_CHANGE,
        FIELD_CHANGE,
        SELECT,
        ALARM,
    };

    Probe &probe;
    Action action;

    ProbeEvent(Probe &probe, Action action) : probe(probe), action(action) { type = Event::Type::PROBE; };
#ifdef DEBUG
    void log(Stream &);
#endif // DEBUG
};

struct InternalTempEvent : Event
{
    int16_t temp;

    InternalTempEvent(int16_t temp) : temp(temp) { type = Event::Type::INT_TEMP; };
#ifdef DEBUG
    void log(Stream &);
#endif
};

void probeSetup();
void probeLoop(uint32_t ts);