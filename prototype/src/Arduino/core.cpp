#include <Arduino.h>
#include "button.h"
#include "event.h"
#include "serial.h"
#include "view.h"

void bbiqSetup() {
    //Serial.begin(9600);
    eventSetup(); // must be first so other setup functions can register event handlers.
    serialSetup();
    buttonSetup();
    viewSetup();
}

void bbiqLoop() {
    serialLoop();
    buttonLoop();
    viewLoop();
}

