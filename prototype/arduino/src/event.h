#ifndef EVENT_E3B0C442_H
#define EVENT_E3B0C442_H

#include <Arduino.h>

//Event IDs
typedef enum {
    SERIAL_RX_EVENT,
    BUTTON_DOWN_EVENT,
    BUTTON_UP_EVENT,
    PROBE_CONNECT_EVENT,
    PROBE_DISCONNECT_EVENT,
    PROBE_CHANGE_EVENT,
    PROBE_ALARM_EVENT,
    LOCAL_INPUT_EVENT,
    UI_CHANGE_EVENT,
    EVENT_COUNT
} EventID;

//Event types
typedef enum {
    GENERIC_EVENT_TYPE,
    SERIAL_EVENT_TYPE,
    BUTTON_EVENT_TYPE,
    PROBE_EVENT_TYPE,
    LOCAL_INPUT_EVENT_TYPE,
    UI_EVENT_TYPE
} EventType;

typedef struct _Event {
    EventID id;
    EventType type;
    unsigned long ts;
    void (*destroy)(struct _Event *);
} Event;

typedef void (*EventHandler)(Event *);

void eventSetup();
void registerHandler(EventID eventID, EventHandler handler);
void dispatch(Event *e);
Event *newGenericEvent(EventID id);
void _destroyGenericEvent(Event* e);

#endif // EVENT_HPP