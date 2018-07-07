#include <Arduino.h>
#include "event.h"

typedef struct {
    byte count;
    EventHandler* handlers;
} _HandlerMap;

_HandlerMap* eventHandlers;

void eventSetup() {
    Serial.print("Event count: ");
    Serial.println(EVENT_COUNT);
    Serial.print("eventHandlers size: ");
    Serial.println(sizeof(_HandlerMap) * EVENT_COUNT);
    eventHandlers = (_HandlerMap*)malloc(sizeof(_HandlerMap) * EVENT_COUNT);
    Serial.print("eventHandlers alloc addr: ");
    Serial.println(long(eventHandlers));
    for(int i = 0; i < EVENT_COUNT; i++) {
        Serial.print("Event ");
        Serial.print(i);
        Serial.print(" handler addres: ");
        Serial.println(long(&eventHandlers[i]));
        eventHandlers[i].count = 0;
        eventHandlers[i].handlers = (EventHandler*) malloc(sizeof(EventHandler*) * 0);
    }
}

void registerHandler(byte eventID, EventHandler handler) {
    Serial.print("Event handler array address: ");
    Serial.println(int(&eventHandlers));
    _HandlerMap* map = &eventHandlers[eventID];
    Serial.print("Handler address: ");
    Serial.println(long(&map));
    map->handlers = (EventHandler*) realloc(map->handlers, sizeof(EventHandler) * (map->count + 1));
    map->handlers[map->count] = handler;
    map->count++;
    Serial.print("Added handler ");
    Serial.print(map->count);
    Serial.print(" for event ");
    Serial.println(eventID);
    for(int i = 0; i < EVENT_COUNT; i++) {
        Serial.print("Event ");
        Serial.print(i);
        Serial.print(" has ");
        Serial.print(eventHandlers[i].count);
        Serial.println(" handlers.");
    }
}

void dispatch(Event* e) {
    for(int i = 0; i < EVENT_COUNT; i++) {
        Serial.print("Event ");
        Serial.print(i);
        Serial.print(" has ");
        Serial.print(eventHandlers[i].count);
        Serial.println(" handlers.");
    }
    Serial.print("Dispatch called for event");
    Serial.println(e->id);
    _HandlerMap map = eventHandlers[e->id];
    Serial.print("Number of handlers for event: ");
    Serial.println(map.count);
    for(int i = 0; i < map.count; i++) {
        Serial.println("About to call a handler");
        (*map.handlers[i])(e);
    }
    e->destroy(e);
}