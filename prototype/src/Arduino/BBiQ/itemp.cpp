#include <Arduino.h>
#include "itemp.h"
#include "pin.h"

float INTERNAL_TEMP;

void readInternal() {
    int internal = analogRead(PIN_TEMP_INTERNAL);
    float intmv = 5000.0/1024.0*float(internal);
    float tempC = (intmv - 500.0) / 10;
    INTERNAL_TEMP = tempC * 1.8 + 32;
}