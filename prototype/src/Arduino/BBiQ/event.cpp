#include <Arduino.h>
#include "event.h"

typedef struct {
    byte count;
    EventHandler* handlers;
} _HandlerMap;

_HandlerMap* eventHandlers = NULL;

void eventSetup() {
    eventHandlers = (_HandlerMap*)malloc(sizeof(_HandlerMap) * EVENT_COUNT);
    for(byte i = 0; i < EVENT_COUNT; i++) {
        eventHandlers[i].count = 0;
        eventHandlers[i].handlers = NULL;
    }
}

void registerHandler(byte eventID, EventHandler handler) {
    _HandlerMap* map = &eventHandlers[eventID];
    map->handlers = (EventHandler*) realloc(map->handlers, sizeof(EventHandler) * (map->count + 1));
    map->handlers[map->count] = handler;
    map->count++;
}

void dispatch(Event* e) {
    Serial.println("GOOD");
    Serial.flush();
    _HandlerMap* map = &eventHandlers[e->id];
    for(int i = 0; i < map->count; i++) {
        map->handlers[i](e);
    }
    e->destroy(e);
}