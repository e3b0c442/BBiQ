#include "event.hpp"

namespace
{
struct HandlerMap
{
    uint8_t count;
    EventHandler *handlers;
};

HandlerMap eventHandlers[(uint8_t)Event::Type::COUNT];
} // namespace

void eventSetup()
{
    for (uint8_t i = 0; i < (uint8_t)Event::Type::COUNT; i++)
    {
        eventHandlers[i].count = 0;
        eventHandlers[i].handlers = NULL;
    }
}

void registerHandler(Event::Type type, EventHandler handler)
{
    HandlerMap *map = &eventHandlers[(uint8_t)type];
    map->handlers = (EventHandler *)realloc(map->handlers, sizeof(EventHandler) * (map->count + 1));
    map->handlers[map->count] = handler;
    map->count++;
}

void dispatch(Event *e)
{
#ifdef DEBUG
    Serial.printf("DEBUG: [%10u] Event dispatch: %d\n", e->ts, e->type);
#endif // DEBUG
    HandlerMap *map = &eventHandlers[(uint8_t)e->type];
    for (uint8_t i = 0; i < map->count; i++)
    {
        map->handlers[i](e);
    }
    delete e;
}