#ifndef PROBE_E3B0C442_H
#define PROBE_E3B0C442_H

#include <Arduino.h>
#include "event.h"
#include "pin.h"

// Steinhart-Hart constants for Thermoworks probes
#define STEIN_A     7.343140544e-4
#define STEIN_B     2.157437229e-4
#define STEIN_C     9.51568577e-8
#define ADC_RES     1024
//#define VREF        5.0
#define STATIC_R    10000.0

// Probe count and default names
const int PROBE_COUNT = 4;
const char* DEFAULT_PROBE_NAMES[] = {
    "Pit",
    "Food 1",
    "Food 2",
    "Food 3"
};
const int DEFAULT_PROBE_PINS[] = {
    PIN_TEMP_PROBE_1,
    PIN_TEMP_PROBE_2,
    PIN_TEMP_PROBE_3,
    PIN_TEMP_PROBE_4 
};

// Loop timers
const unsigned long PROBES_POWERON_DELAY = 1;
const unsigned long PROBES_READ_INTERVAL = 1000;
extern unsigned long PROBES_POWERED_ON_TIME;
extern unsigned long PROBES_LAST_READ_TIME;
extern float INTERNAL_TEMPERATURE;

// Probe information. lowAlarm/highAlarm < 0 means not set/alarming
typedef struct {
    byte id;
    byte pin;
    bool connected;
    float temperature;
    int lowAlarm;
    int highAlarm;
    const char* name;
} Probe;

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

extern Probe** probes;

void            probeSetup();
void            probeLoop(Probe** probes, int len);
unsigned long   powerOnProbes();
void            powerOffProbes();
unsigned long   readProbes(Probe** probes, int len);
void            readProbe(Probe* probe);
ProbeEvent*     newProbeEvent(byte eventID, Probe* probe);
void            _destroyProbeEvent(Event* e);

#endif //PROBE_E3B0C442_H

