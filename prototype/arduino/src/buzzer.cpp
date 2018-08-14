#pragma GCC diagnostic ignored "-Wswitch"
#include "buzzer.h"
#include "event.h"
#include "pin.h"

enum {
    BEEP_SHORT,
    BEEP_LONG
};

const unsigned int BUZZER_FREQUENCY = 2700U;
const byte BEEP_SEQUENCE = 4;
const unsigned long BEEP_LENGTH = 100UL;
const unsigned long BEEP_SHORT_INTERVAL = 200UL;
const unsigned long BEEP_LONG_INTERVAL = 1000UL;

byte beepCount = 0;
byte nextInterval = BEEP_SHORT;
unsigned long lastBeep = 0UL;

bool buzzerAlarming = false;

void buzzerEventHandler(Event *e);

void buzzerSetup() {
    pinMode(PIN_BUZZER, OUTPUT);
    registerHandler(PROBE_ALARM_EVENT, &buzzerEventHandler);
    registerHandler(BUTTON_DOWN_EVENT, &buzzerEventHandler);
}

void buzzerLoop() {
    if(buzzerAlarming) {
        unsigned long ts = millis();
        switch(nextInterval) {
            case BEEP_SHORT:
                if((long)ts - long(lastBeep) < (long)BEEP_SHORT_INTERVAL) {
                    return;
                }
                break;
            case BEEP_LONG:
                if((long)ts - long(lastBeep) < (long)BEEP_LONG_INTERVAL) {
                    return;
                }
                nextInterval = BEEP_SHORT;
                break;
        }
        lastBeep = ts;
        beepCount++;
        if(beepCount >= BEEP_SEQUENCE) {
            beepCount = 0;
            nextInterval = BEEP_LONG;
        }
        tone(PIN_BUZZER, BUZZER_FREQUENCY, BEEP_LENGTH);
    }
}

void buzzerEventHandler(Event *e) {
    switch(e->id) {
        case PROBE_ALARM_EVENT:
            if(!buzzerAlarming) {
                buzzerAlarming = true;
            }
            break;
        case BUTTON_DOWN_EVENT:
            if(buzzerAlarming) {
                buzzerAlarming = false;
            }
            break;
    }
}