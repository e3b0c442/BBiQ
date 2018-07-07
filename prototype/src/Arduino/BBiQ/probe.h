#ifndef PROBE_E3B0C442_H
#define PROBE_E3B0C442_H

#include <Arduino.h>
#include "event.h"
#include "pin.h"

enum {
    PROBE_0,
    PROBE_1,
    PROBE_2,
    PROBE_3
};

typedef struct _ProbeEvent {
    Event event;
    byte probe;
    bool connected;
    float temperature;
    int lowAlarm;
    int highAlarm;
    const char* name;
    void (*destroy)(struct _ProbeEvent*);
} ProbeEvent;

void            probeSetup();
void            probeLoop();

#endif //PROBE_E3B0C442_H

