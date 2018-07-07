#ifndef EVENT_E3B0C442_H
#define EVENT_E3B0C442_H

#include <Arduino.h>

typedef struct _Event {
    byte id;
    byte type;
    unsigned long ts;
    void (*destroy)(struct _Event*);
} Event;
typedef void (*EventHandler)(Event*);

//Event IDs
enum {
    SERIAL_RX_EVENT,
    BUTTON_DOWN_EVENT,
    BUTTON_UP_EVENT,
    PROBE_CONNECT_EVENT,
    PROBE_DISCONNECT_EVENT,
    PROBE_CHANGE_EVENT,
    PROBE_ALARM_EVENT,
    EVENT_COUNT // used to determine length of list; must always be last
};

//Event types
enum {
    SERIAL_EVENT_TYPE,
    BUTTON_EVENT_TYPE,
    PROBE_EVENT_TYPE
};

void eventSetup();
void registerHandler(byte eventID, EventHandler handler);
void dispatch(Event* e);

#endif // EVENT_HPP