#include <Arduino.h>
#include "event.h"

typedef struct {
    byte count;
    EventHandler *handlers;
} _HandlerMap;

_HandlerMap eventHandlers[EVENT_COUNT];

void _destroyGenericEvent(Event* e);

void eventSetup() {
    for(byte i = 0; i < EVENT_COUNT; i++) {
        eventHandlers[i].count = 0;
        eventHandlers[i].handlers = NULL;
    }
}

void registerHandler(EventID eventID, EventHandler handler) {
    _HandlerMap *map = &eventHandlers[eventID];
    map->handlers = (EventHandler *)realloc(map->handlers, sizeof(EventHandler) * (map->count + 1));
    map->handlers[map->count] = handler;
    map->count++;
}

void dispatch(Event *e) {
    Serial.print("Event triggered: ");
    Serial.println((byte)e->id);
    _HandlerMap *map = &eventHandlers[e->id];
    for(int i = 0; i < map->count; i++) {
        map->handlers[i](e);
    }
    e->destroy(e);
}

Event* newGenericEvent(EventID id) {
    Event* e = (Event *)malloc(sizeof(Event));
    e->id = id;
    e->type = GENERIC_EVENT_TYPE;
    e->ts = millis();
    e->destroy = _destroyGenericEvent;
    return e;
}

void _destroyGenericEvent(Event* e) {
    free(e);
}
