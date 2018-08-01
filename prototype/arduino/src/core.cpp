#include <Arduino.h>
#include "button.h"
#include "buzzer.h"
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
    buzzerSetup();
    displaySetup();
}

void bbiqLoop() {
    serialLoop();
    buttonLoop();
    probeLoop();
    uiLoop();
    buzzerLoop();
    displayLoop();
}

