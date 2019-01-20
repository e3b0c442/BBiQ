#include <Arduino.h>
#include "event.h"

namespace
{
struct _HandlerMap
{
    byte count;
    EventHandler *handlers;
};

_HandlerMap eventHandlers[Event::COUNT];
}; // namespace

void eventSetup()
{
    for (byte i = 0; i < Event::COUNT; i++)
    {
        eventHandlers[i].count = 0;
        eventHandlers[i].handlers = NULL;
    }
}

void registerHandler(Event::Type type, EventHandler handler)
{
    _HandlerMap *map = &eventHandlers[type];
    map->handlers = (EventHandler *)realloc(map->handlers, sizeof(EventHandler) * (map->count + 1));
    map->handlers[map->count] = handler;
    map->count++;
}

void dispatch(Event *e)
{
    _HandlerMap *map = &eventHandlers[e->type];
    for (int i = 0; i < map->count; i++)
    {
        map->handlers[i](e);
    }
    delete e;
}

Event::Event()
{
    type = GENERIC_TYPE;
    ts = millis();
}