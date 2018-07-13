#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include "crc.h"
#include "event.h"
#include "serial.h"

#define SERIAL_TX_START         0x01
#define SERIAL_TX_END           0x04
#define SERIAL_PAYLOAD_START    0x02
#define SERIAL_PAYLOAD_END      0x03
#define SERIAL_PAYLOAD_START_S  "\x02"
#define SERIAL_PAYLOAD_END_S    "\x03"

byte receive(byte* payload);
SerialEvent* newSerialEvent(EventID eventID, byte* data);
void _destroySerialEvent(Event* evt);

void serialSetup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Serial.println(F("Begin"));
}

void serialLoop() {
    while(Serial.available() > 0) {
        if(Serial.read() == SERIAL_TX_START) {
            byte* payload = (byte*) malloc(SERIAL_RX_BUFFER_SIZE + 1);
            int len = receive(payload);
            if(len == 0) {
                break;
            }
            payload = (byte*) realloc(payload, len + 1); // free some heap
            dispatch((Event*) newSerialEvent(SERIAL_RX_EVENT, payload));
            break;
        }
    }
}

byte receive(byte* rx) {
    // read from Serial and validate the transmission
    byte len = Serial.readBytesUntil(SERIAL_TX_END, rx, SERIAL_RX_BUFFER_SIZE);
    rx[len] = 0; // add null terminator
    if(rx[1] != SERIAL_PAYLOAD_START || rx[len - 1] != SERIAL_PAYLOAD_END) {
        free(rx);
        return 0;
    }

    // tokenize the transmission
    byte cksum = rx[0];
    strtok((char*) rx, SERIAL_PAYLOAD_START_S);
    char* payload = strtok(NULL, SERIAL_PAYLOAD_END_S);
    byte payloadLen = strlen(payload);
    
    // validate the checksum
    if(!crcVerify(cksum, (byte*) payload, payloadLen)) {
        free(rx);
        return 0;
    }

    // rearrange the bytes so that the buffer contains only the payload
    memmove(rx, payload, payloadLen + 1);
    return payloadLen;
}

SerialEvent* newSerialEvent(EventID eventID, byte* data) {
    SerialEvent* e = (SerialEvent*) malloc(sizeof(SerialEvent));
    e->event = {
        .id = eventID,
        .type = SERIAL_EVENT_TYPE,
        .ts = millis(),
        .destroy = _destroySerialEvent
    };
    e->data = data;
    return e;
}

void _destroySerialEvent(Event* e) {
    SerialEvent* evt = (SerialEvent*) e;
    free(evt->data);
    free(e);
}