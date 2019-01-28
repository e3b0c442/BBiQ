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

    Probe *probe;
    Action action;

    ProbeEvent(Probe *aProbe, Action anAction)
    {
        type = Event::Type::PROBE, probe = aProbe, action = anAction;
    };
};

void probeSetup();
void probeLoop(uint32_t *ts);
