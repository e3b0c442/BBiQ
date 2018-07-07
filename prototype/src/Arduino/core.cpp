#include <Arduino.h>
#include "serial.h"
#include "view.h"

void bbiqSetup() {
    serialSetup();
    viewSetup();
}

void bbiqLoop() {
    serialLoop();
    viewLoop();
}

