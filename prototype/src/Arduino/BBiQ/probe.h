#ifndef PROBE_E3B0C442_H
#define PROBE_E3B0C442_H

#include <Arduino.h>
#include "event.h"
#include "pin.h"

typedef enum {
    PROBE_0,
    PROBE_1,
    PROBE_2,
    PROBE_3,
    PROBE_COUNT
} ProbeID;

typedef struct {
    ProbeID id;
    byte pin;
    bool connected;
    float temperature;
    int lowAlarm;
    int highAlarm;
    const char* name;
} Probe;

extern Probe* probes;
extern byte probeConnectedCount;

typedef struct _ProbeEvent {
    Event event;
    ProbeID probe;
    void (*destroy)(struct _ProbeEvent*);
} ProbeEvent;

void probeSetup();
void probeLoop();

#endif //PROBE_E3B0C442_H

