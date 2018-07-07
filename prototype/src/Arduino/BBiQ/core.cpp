#include <Arduino.h>
#include "button.h"
#include "event.h"
#include "serial.h"
#include "view.h"

#include "freeMem.h"

void bbiqSetup() {
    Serial.begin(9600);
    Serial.println("Event setup");
    eventSetup(); // must be first so other setup functions can register event handlers.
    delay(100);
    //Serial.println("Serial setup");
    serialSetup();
    //delay(100);
    //Serial.println("Button setup");
    buttonSetup();
   // delay(100);
   // Serial.println("View setup");
    viewSetup();
    //delay(100);
    //Serial.println("Setup complete");
}

void bbiqLoop() {
    Serial.print("Free memory: ");
    Serial.println(freeMemory());
    delay(100);
    Serial.println("Serial loop");
    serialLoop();
    delay(100);
    Serial.println("Button loop");
    buttonLoop();
    delay(100);
    Serial.println("View loop");
    viewLoop();
}

