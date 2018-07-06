#ifndef EVENT_E3B0C442_H
#define EVENT_E3B0C442_H

typedef struct _Event {
    byte id;
    byte type;
    unsigned long ts;
    void (*destroy)(struct _Event*);
} Event;
typedef void (*EventHandler)(Event);

//Event IDs
const byte SERIAL_RX_EVENT = 0x01;
const byte BUTTON_DOWN_EVENT = 0x02;
const byte BUTTON_UP_EVENT = 0x03;
const byte PROBE_CONNECT_EVENT = 0x04;
const byte PROBE_DISCONNECT_EVENT = 0x05;
const byte PROBE_CHANGE_EVENT = 0x06;
const byte PROBE_ALARM_EVENT = 0x07;

//Event types
const byte SERIAL_EVENT_TYPE = 0x01;
const byte BUTTON_EVENT_TYPE = 0x02;
const byte PROBE_EVENT_TYPE = 0x03;

const byte Events[] = {
    SERIAL_RX_EVENT,
    BUTTON_DOWN_EVENT,
    BUTTON_UP_EVENT,
    PROBE_CONNECT_EVENT,
    PROBE_DISCONNECT_EVENT,
    PROBE_CHANGE_EVENT,
    PROBE_ALARM_EVENT
};

void dispatch(Event* e);

#endif // EVENT_HPP