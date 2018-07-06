#include <Arduino.h>
#include "event.h"
#include "pin.h"
#include "probe.h"

unsigned long PROBES_POWERED_ON_TIME;
unsigned long PROBES_LAST_READ_TIME;

Probe** probes;

void probeSetup() {
    //Initialize the probe array
    pinMode(PIN_PROBE_POWER, OUTPUT);
    probes = (Probe**) malloc(sizeof(Probe*) * PROBE_COUNT);
    for(int i = 0; i < PROBE_COUNT; i++) {
        probes[i] = (Probe*) malloc(sizeof(Probe));
        probes[i]->name = DEFAULT_PROBE_NAMES[i];
        probes[i]->pin = DEFAULT_PROBE_PINS[i];
        probes[i]->connected = false;
        probes[i]->temperature = 0.0;
        probes[i]->lowAlarm = 140.0;
        probes[i]->highAlarm = 160.0;
    }
}

unsigned long powerOnProbes() {
    digitalWrite(PIN_PROBE_POWER, HIGH);
    return millis();
}

void powerOffProbes() {
    digitalWrite(PIN_PROBE_POWER, LOW);
}

unsigned long readProbes(Probe** probes, int len) {
    digitalWrite(PIN_PROBE_POWER, HIGH);
    for(int i = 0; i < len; i++) {
        readProbe(probes[i]);
    }
    return millis();
}

void readProbe(Probe* probe) {
    int x = analogRead(probe->pin);
    bool oldConn = probe->connected;
    float oldTemp = probe->temperature;
    if((x == 0) || (x == ADC_RES - 1)) {
        probe->connected = false;
        probe->temperature = 0;
        if(oldConn) {
            dispatch((Event*)newProbeEvent(PROBE_DISCONNECT_EVENT, probe));
        }
        return;
    }
    float res = float(STATIC_R) / ((float(ADC_RES - 1)/float(x-1)) - 1);
    float kelvins = 1.0/(float(STEIN_A) + float(STEIN_B*log(res)) + float(STEIN_C*pow(log(res), 3.0)));
    float temp = 1.8 * (kelvins - 273.0) + 32.0;
    probe->connected = true;
    probe->temperature = temp;
    if(!oldConn) {
        dispatch((Event*)newProbeEvent(PROBE_CONNECT_EVENT, probe));
    }
    if(probe->temperature != oldTemp) {
        dispatch((Event*)newProbeEvent(PROBE_CHANGE_EVENT, probe));
    }
}

void probeLoop(Probe** probes, int len) {
    unsigned long loopTime = millis();
    if(long(PROBES_POWERED_ON_TIME) - long(PROBES_LAST_READ_TIME) > 0) {
        if(long(loopTime) - long(PROBES_POWERED_ON_TIME) > PROBES_POWERON_DELAY) {
            PROBES_LAST_READ_TIME = readProbes(probes, len);
            powerOffProbes();
        }
    } else {
        if(loopTime - PROBES_LAST_READ_TIME > PROBES_READ_INTERVAL) {
            PROBES_POWERED_ON_TIME = powerOnProbes();
        }
    }
}

ProbeEvent* newProbeEvent(byte eventID, Probe* probe) {
    ProbeEvent* e = (ProbeEvent*) malloc(sizeof(ProbeEvent));
    e->event = {
        eventID,
        PROBE_EVENT_TYPE,
        millis(),
        _destroyProbeEvent
    };
    e->probe = probe->id;
    e->connected = probe->connected;
    e->temperature = probe->temperature;
    e->highAlarm = probe->highAlarm;
    e->lowAlarm = probe->lowAlarm;
}

void _destroyProbeEvent(Event* e) {
    free(e);
}