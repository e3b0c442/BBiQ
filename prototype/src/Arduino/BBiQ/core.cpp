#include <Arduino.h>
#include "button.h"
#include "event.h"
#include "serial.h"
#include "view.h"

#include "freeMem.h"

void bbiqSetup() {
    eventSetup(); // must be first so other setup functions can register event handlers.
    serialSetup();
    buttonSetup();
    viewSetup();
}

void bbiqLoop() {
    Serial.print("Free memory: ");
    Serial.println(freeMemory());
    serialLoop();
    buttonLoop();
    viewLoop();
}

