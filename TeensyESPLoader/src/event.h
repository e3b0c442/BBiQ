#ifndef EVENT_E3B0C442_H
#define EVENT_E3B0C442_H

#include <Arduino.h>

struct Event
{
    enum Type
    {
        GENERIC_TYPE,
        BUTTON_STATE_TYPE,
        //SERIAL_RX_EVENT,
        //BUTTON_DOWN_EVENT,
        //BUTTON_UP_EVENT,
        //PROBE_CONNECT_EVENT,
        //PROBE_DISCONNECT_EVENT,
        //PROBE_CHANGE_EVENT,
        //PROBE_ALARM_EVENT,
        //LOCAL_INPUT_EVENT,
        //UI_CHANGE_EVENT,
        //UI_CHANGE_SCREEN_EVENT,
        COUNT
    };

    Type type;
    unsigned long ts;

    Event();
};

typedef void (*EventHandler)(Event *);

void eventSetup();
void registerHandler(Event::Type type, EventHandler handler);
void dispatch(Event *e);

#endif // EVENT_E3B0C442_H