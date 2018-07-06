#include <Arduino.h>
#include "button.h"
#include "event.h"
#include "pin.h"

Button** buttons;

void buttonSetup() {
    buttons = (Button**) malloc(sizeof(Button*) * BUTTON_COUNT);
    for(int i = 0; i < BUTTON_COUNT; i++) {
        pinMode(BUTTON_PINS[i], INPUT_PULLUP);
        buttons[i] = (Button*) malloc(sizeof(Button));
        buttons[i]->id = BUTTON_IDS[i];
        buttons[i]->name = BUTTON_NAMES[i];
        buttons[i]->pin = BUTTON_PINS[i];
        buttons[i]->state = HIGH;
        buttons[i]->reading = HIGH;
        buttons[i]->repeat = false;
        buttons[i]->lastChange = 0UL;
        buttons[i]->lastRepeat = 0UL;
    }
}

void buttonLoop() {
    unsigned long ts = millis();
    readButtons(buttons, BUTTON_COUNT, ts);
}

void readButtons(Button** buttons, int len, unsigned long ts) {
    for(int i = 0; i < len; i++) {
        readButton(buttons[i], ts);
    }
}

void readButton(Button* button, unsigned long ts) {
    bool reading = digitalRead(button->pin);
    if(reading != button->reading) {
        button->lastChange = ts;
        button->reading = reading;
    }
    if(button->reading != button->state) {
        if(long(ts) - long(button->lastChange) > long(BUTTON_DEBOUNCE_DELAY)) {
            button->state = button->reading;
            button->repeat = false;
            dispatch((Event*)newButtonEvent((button->state == HIGH ? BUTTON_UP_EVENT : BUTTON_DOWN_EVENT), button));
        }
    } else {
        if(button->state == LOW) {
            if(long(ts) - long(button->lastChange) > long(BUTTON_INITIAL_REPEAT_DELAY)) {
                if(long(ts) - long(button->lastRepeat) > long(BUTTON_REPEAT_DELAY)) {
                    button->lastRepeat = ts;
                    button->repeat = true;
                    dispatch((Event*)newButtonEvent(BUTTON_DOWN_EVENT, button));
                }
            }
        }
    }
}

ButtonEvent* newButtonEvent(byte eventID, Button* button) {
    ButtonEvent* e = (ButtonEvent*) malloc(sizeof(ButtonEvent));
    e->event = {
        eventID,
        BUTTON_EVENT_TYPE,
        millis(),
        _destroyButtonEvent
    };
    e->button = button->id;
    e->state = button->state;
    e->repeat = button->repeat;
    return e;
}

void _destroyButtonEvent(Event* e) {
    free(e);
}