#ifndef BUTTON_E3B0C442_H
#define BUTTON_E3B0C442_H

#include <Arduino.h>
#include "event.h"
#include "pin.h"

const byte BUTTON_1 = 0x0;
const byte BUTTON_2 = 0x1;
const byte BUTTON_3 = 0x2;

const unsigned long BUTTON_DEBOUNCE_DELAY = 10;
const unsigned long BUTTON_REPEAT_DELAY = 500;
const unsigned long BUTTON_INITIAL_REPEAT_DELAY = 1000;

// Button count and default names
const byte BUTTON_COUNT = 3;
const byte BUTTON_IDS[] = {
    BUTTON_1,
    BUTTON_2,
    BUTTON_3
};
const String BUTTON_NAMES[] = {
    "Button 1",
    "Button 2",
    "Button 3"
};
const byte BUTTON_PINS[] = {
    PIN_BUTTON_1,
    PIN_BUTTON_2,
    PIN_BUTTON_3
};

typedef struct {
    byte id;
    String name;
    byte pin;
    bool state;
    bool reading;
    bool repeat;
    unsigned long lastChange;
    unsigned long lastRepeat;
} Button;

typedef struct {
    Event event;
    byte button;
    bool state;
    bool repeat;
} ButtonEvent;

extern Button** buttons;

void buttonSetup();
void buttonLoop();
void readButtons(Button** buttons, int len, unsigned long ms);
void readButton(Button* button, unsigned long ms);
ButtonEvent* newButtonEvent(byte eventID, Button* button);
void _destroyButtonEvent(Event* evt);

#endif // BUTTON_E3B0C442_H