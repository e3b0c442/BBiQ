#include <Arduino.h>
#include "event.h"
#include "pin.h"
#include "probe.h"

// Steinhart-Hart constants for Thermoworks probes
#define STEIN_A     7.343140544e-4
#define STEIN_B     2.157437229e-4
#define STEIN_C     9.51568577e-8
#define ADC_RES     1024
#define STATIC_R    10000.0

// Probe count and default names
const char PROBE_0_DEFAULT_NAME[] PROGMEM = "Pit";
const char PROBE_1_DEFAULT_NAME[] PROGMEM = "Food 1";
const char PROBE_2_DEFAULT_NAME[] PROGMEM = "Food 2";
const char PROBE_3_DEFAULT_NAME[] PROGMEM = "Food 3";

const char *const PROBE_DEFAULT_NAMES[] PROGMEM = {
    PROBE_0_DEFAULT_NAME,
    PROBE_1_DEFAULT_NAME,
    PROBE_2_DEFAULT_NAME,
    PROBE_3_DEFAULT_NAME
};

const byte PROBE_PINS[] = {
    PIN_TEMP_PROBE_0,
    PIN_TEMP_PROBE_1,
    PIN_TEMP_PROBE_2,
    PIN_TEMP_PROBE_3
};

byte probeConnectedCount = 0;

// Loop timers
const unsigned long PROBES_POWERON_DELAY = 1;
const unsigned long PROBES_READ_INTERVAL = 1000;
unsigned long PROBES_POWERED_ON_TIME;
unsigned long PROBES_LAST_READ_TIME;

Probe *probes;

unsigned long powerOnProbes();
void powerOffProbes();
unsigned long readProbes();
void readProbe(Probe *probe);
ProbeEvent *newProbeEvent(EventID eventID, ProbeID probe);
void _destroyProbeEvent(Event *e);

void probeSetup() {
    //Initialize the probe array
    pinMode(PIN_PROBE_POWER, OUTPUT);
    probeConnectedCount = 0;
    probes = (Probe*)malloc(sizeof(Probe) * PROBE_COUNT);
    for(byte i = 0; i < PROBE_COUNT; i++) {
        char *name = (char *)malloc(sizeof(char)*(strlen_P((char *)pgm_read_word(&(PROBE_DEFAULT_NAMES[i]))) + 1));
        strcpy_P(name, (char *)pgm_read_word(&(PROBE_DEFAULT_NAMES[i])));
        probes[i].id = (ProbeID)i;
        probes[i].name = name;
        probes[i].pin = PROBE_PINS[i];
        probes[i].connected = false;
        probes[i].temperature = 0.0;
        probes[i].lowAlarm = 140.0;
        probes[i].highAlarm = 160.0;
    }
}

unsigned long powerOnProbes() {
    digitalWrite(PIN_PROBE_POWER, HIGH);
    return millis();
}

void powerOffProbes() {
    digitalWrite(PIN_PROBE_POWER, LOW);
}

unsigned long readProbes() {
    digitalWrite(PIN_PROBE_POWER, HIGH);
    for(int i = 0; i < PROBE_COUNT; i++) {
        readProbe(&probes[i]);
    }
    return millis();
}

void readProbe(Probe *probe) {
    int x = analogRead(probe->pin);
    bool oldConn = probe->connected;
    float oldTemp = probe->temperature;
    if((x == 0) || (x == ADC_RES - 1)) {
        probe->connected = false;
        probe->temperature = 0;
        if(oldConn) {
            probeConnectedCount--;
            dispatch((Event*)newProbeEvent(PROBE_DISCONNECT_EVENT, probe->id));
        }
        return;
    }
    float res = float(STATIC_R) / ((float(ADC_RES - 1)/float(x-1)) - 1);
    float kelvins = 1.0/(float(STEIN_A) + float(STEIN_B*log(res)) + float(STEIN_C*pow(log(res), 3.0)));
    float temp = 1.8 * (kelvins - 273.0) + 32.0;
    probe->connected = true;
    probe->temperature = temp;
    if(!oldConn) {
        probeConnectedCount++;
        dispatch((Event*)newProbeEvent(PROBE_CONNECT_EVENT, probe->id));
    }
    if(probe->temperature != oldTemp) {
        dispatch((Event*)newProbeEvent(PROBE_CHANGE_EVENT, probe->id));
    }
}

void probeLoop() {
    unsigned long loopTime = millis();
    if(long(PROBES_POWERED_ON_TIME) - long(PROBES_LAST_READ_TIME) > 0) {
        if(long(loopTime) - long(PROBES_POWERED_ON_TIME) > PROBES_POWERON_DELAY) {
            PROBES_LAST_READ_TIME = readProbes();
            powerOffProbes();
        }
    } else {
        if(loopTime - PROBES_LAST_READ_TIME > PROBES_READ_INTERVAL) {
            PROBES_POWERED_ON_TIME = powerOnProbes();
        }
    }
}

ProbeEvent *newProbeEvent(EventID eventID, ProbeID probe) {
    ProbeEvent *e = (ProbeEvent *)malloc(sizeof(ProbeEvent));
    e->event = {
        .id = eventID,
        .type = PROBE_EVENT_TYPE,
        .ts = millis(),
        .destroy = _destroyProbeEvent
    };
    e->probe = probe;
    return e;
}

void _destroyProbeEvent(Event *e) {
    free(e);
}