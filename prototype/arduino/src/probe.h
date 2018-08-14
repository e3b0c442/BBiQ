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

typedef enum {
    PROBE_FIELD_NAME,
    PROBE_FIELD_TEMP,
    PROBE_FIELD_LOW,
    PROBE_FIELD_HIGH,
    PROBE_FIELD_COUNT
} ProbeFieldID;

typedef struct {
    ProbeID id;
    byte pin;
    bool connected;
    float temperature;
    int lowAlarm;
    int highAlarm;
    bool arm;
    const char *name;
} Probe;

extern Probe *probes;
extern byte probeConnectedCount;

typedef struct _ProbeEvent {
    Event event;
    ProbeID probe;
    ProbeFieldID field;
    void (*destroy)(struct _ProbeEvent *);
} ProbeEvent;

void probeSetup();
void probeLoop();

#endif //PROBE_E3B0C442_H

