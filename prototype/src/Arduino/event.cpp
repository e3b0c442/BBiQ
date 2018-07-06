#include <Arduino.h>
#include "event.h"

void dispatch(Event* e) {
    Serial.print(F("Dispatcher received event id "));
    Serial.print(e->id);
    Serial.print(F(" type "));
    Serial.println(e->type);
}