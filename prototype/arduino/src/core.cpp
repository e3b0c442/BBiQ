#include <Arduino.h>
#include "button.h"
#include "display.h"
#include "event.h"
#include "probe.h"
#include "serial.h"
#include "ui.h"

#include "freeMem.h"

unsigned long lastMemMillis = 0UL;

void bbiqSetup() {
    eventSetup(); // must be first so other setup functions can register event handlers.
    serialSetup();
    buttonSetup();
    probeSetup();
    uiSetup();
    displaySetup();
}

void bbiqLoop() {
    long ms;
    if((ms = (long)millis()) - (long)lastMemMillis > 1000L) {
        lastMemMillis = ms;
        Serial.print("Free memory: ");
        Serial.println(freeMemory());
        Serial.flush();
    }
    serialLoop();
    buttonLoop();
    probeLoop();
    uiLoop();
    displayLoop();
}

