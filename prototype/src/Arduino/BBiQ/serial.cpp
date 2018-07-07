#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include "crc.h"
#include "event.h"
#include "serial.h"

String serialBuffer;

void serialSetup() {
    //Serial.begin(SERIAL_BAUD_RATE);
    Serial.println(F("Begin"));
}

void serialLoop() {
    while(Serial.available() > 0) {
        if(Serial.read() == SERIAL_TX_START) {
            byte* payload = (byte*) malloc(SERIAL_BUFFER_LEN + 1);
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

byte receive(byte* payload) {
    // read from Serial and validate the transmission
    byte len = Serial.readBytesUntil(SERIAL_TX_END, payload, SERIAL_BUFFER_LEN);
    payload[len] = 0; // add null terminator
    if(payload[1] != SERIAL_PAYLOAD_START || payload[len - 1] != SERIAL_PAYLOAD_END) {
        free(payload);
        return 0;
    }

    // tokenize the transmission
    byte cksum = payload[0];
    strtok((char*) payload, SERIAL_PAYLOAD_START_S);
    char* _payload = strtok(NULL, SERIAL_PAYLOAD_END_S);
    byte payloadLen = strlen(_payload);
    
    // validate the checksum
    if(!crcVerify(cksum, (byte*) _payload, payloadLen)) {
        free(payload);
        return 0;
    }

    // rearrange the bytes so that the buffer contains only the payload
    memmove(payload, _payload, payloadLen + 1);
    return payloadLen;
}

SerialEvent* newSerialEvent(byte eventID, byte* data) {
    SerialEvent* e = (SerialEvent*) malloc(sizeof(SerialEvent));
    e->event = {
        eventID,
        SERIAL_EVENT_TYPE,
        millis(),
        _destroySerialEvent
    };
    e->data = data;
    return e;
}

void _destroySerialEvent(Event* e) {
    SerialEvent* evt = (SerialEvent*) e;
    free(evt->data);
    free(e);
}