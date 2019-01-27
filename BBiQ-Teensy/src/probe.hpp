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

    enum class Field : uint8_t
    {
        NAME,
        TEMP,
        LOW_SET,
        HIGH_SET,
        NA,
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

extern Probe probes[(uint8_t)Probe::ID::COUNT];
extern uint8_t probeConnectedCount;

struct ProbeEvent : Event
{
    enum class Action : uint8_t
    {
        CONNECT,
        DISCONNECT,
        FIELD_CHANGE,
        ALARM,
    };

    Probe::ID probe;
    Probe::Field field;
    Action action;

    ProbeEvent(Probe::ID aProbe, Probe::Field aField, Action anAction)
    {
        type = Event::Type::PROBE, probe = aProbe, field = aField, action = anAction;
    };
};

void probeSetup();
void probeLoop(uint32_t *ts);
