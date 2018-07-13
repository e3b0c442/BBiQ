#ifndef BUTTON_E3B0C442_H
#define BUTTON_E3B0C442_H

#include <Arduino.h>
#include "event.h"
#include "pin.h"

typedef enum {
    BUTTON_0,
    BUTTON_1,
    BUTTON_2
} ButtonID;

typedef struct {
    Event event;
    ButtonID button;
    bool state;
    bool repeat;
} ButtonEvent;

void buttonSetup();
void buttonLoop();

#endif // BUTTON_E3B0C442_H