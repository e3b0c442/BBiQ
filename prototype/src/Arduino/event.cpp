#include <Arduino.h>
#include "event.h"

typedef struct {
    byte count;
    EventHandler* handlers;
} _HandlerMap;

_HandlerMap* eventHandlers;

void eventSetup() {
    eventHandlers = (_HandlerMap*)malloc(sizeof(_HandlerMap) * EVENT_COUNT);
    for(int i = 0; i < EVENT_COUNT; i++) {
        eventHandlers[i].count = 0;
        eventHandlers[i].handlers = (EventHandler*) malloc(sizeof(EventHandler*) * 0);
    }
}

void registerHandler(byte eventID, EventHandler handler) {
    _HandlerMap map = eventHandlers[eventID];
    map.handlers = (EventHandler*) realloc(map.handlers, sizeof(EventHandler*) * (map.count + 1));
    map.handlers[map.count] = handler;
    map.count++;
    Serial.print("Added handler ");
    Serial.print(map.count);
    Serial.print(" for event ");
    Serial.println(eventID);

}

void dispatch(Event* e) {
    Serial.println("Dispatch called");
    _HandlerMap map = eventHandlers[e->id];
    Serial.print("Number of handlers for event: ");
    Serial.println(map.count);
    for(int i = 0; i < map.count; i++) {
        Serial.println("About to call a handler");
        (*map.handlers[i])(e);
    }
}