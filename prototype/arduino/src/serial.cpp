#include <Firmata.h>
#include "crc.h"
#include "probe.h"
#include "serial.h"

#define SOH     0x01
#define STX     0x02
#define ETX     0x03
#define EOT     0x04
#define SOH_S   "\x01"
#define STX_S   "\x02"
#define ETX_S   "\x03"
#define EOT_S   "\x04"

#define SYSEX_BBIQ_MESSAGE 0x00

void sysexCallback(byte cmd, byte count, byte* data);
SerialEvent *newSerialEvent(byte cmd, byte count, byte *data);
void _destroySerialEvent(Event *e);

void serialSetup() {
    Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);
    Firmata.attach(START_SYSEX, &sysexCallback);
    Firmata.begin();
}

void serialLoop() {
    while(Firmata.available()) {
        Firmata.processInput();
    }
}

void sysexCallback(byte cmd, byte count, byte* data) {
    // copy the data out of the Firmata buffer
    char *cdata = (char *)malloc(sizeof(char) * (count + 1));
    memcpy(cdata, data, count);
    if(cdata[1] != SOH || cdata[count - 1] != EOT) {
        free(cdata);
        return;
    }

    //tokenize
    strtok(cdata, SOH_S);
    char *rx = strtok(NULL, EOT_S);
    if(rx == NULL) {
        free(cdata);
        return;
    }
    byte cksum = (byte)rx[0];

    strtok(rx, STX_S);
    char *payload = strtok(NULL, ETX_S);
    if(payload == NULL) {
        free(cdata);
        return;
    }
    byte payloadCount = strlen(payload);

    //validate CRC
    if(!crcVerify(cksum, (byte *)payload, payloadCount)) {
        free(cdata);
        return;
    }

    memmove(cdata, payload, payloadCount);
    dispatch((Event *)newSerialEvent(cmd, payloadCount, (byte *)payload));
}

SerialEvent *newSerialEvent(byte cmd, byte count, byte *data) {
    SerialEvent *e = (SerialEvent *)malloc(sizeof(SerialEvent));
    e->event = {
        .id = SERIAL_RX_EVENT,
        .type = SERIAL_EVENT_TYPE,
        .ts = millis(),
        .destroy = _destroySerialEvent
    };
    e->cmd = cmd;
    e->count = count;
    e->data = data;
    return e;
}

void _destroySerialEvent(Event *e) {
    SerialEvent *evt = (SerialEvent *)e;
    free(evt->data);
    free(e);
}

/*
void handleFirmataString(char *str) {
    //copy from internal buffer
    char *payload = (char *)malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(payload, str);

    //tokenize
    char *probe_str = strtok(payload, SERIAL_FIELD_SEPARATOR);
    if(probe_str == NULL) {
        return;
    }
    char *field_str = strtok(NULL, SERIAL_FIELD_SEPARATOR);
    if(field_str == NULL) {
        return;
    }
    char *data_str = strtok(NULL, SERIAL_FIELD_SEPARATOR);
    if(data_str == NULL) {
        return;
    }

    ProbeID probeID = (ProbeID)atoi(probe_str);
    if(probeID == 0 && probe_str[0] != '0') {
        return;
    }
    ProbeFieldID fieldID = (ProbeFieldID)atoi(field_str);
    if(fieldID == 0 && field_str[0] != '0') {
        return;
    }


}*/
/*#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include "crc.h"
#include "event.h"
#include "serial.h"


byte receive(byte *payload);
SerialEvent *newSerialEvent(EventID eventID, byte *data);
void _destroySerialEvent(Event *evt);

void serialSetup() {
    Serial.begin(SERIAL_BAUD_RATE);
}

void serialLoop() {
    while(Serial.available() > 0) {
        if(Serial.read() == SERIAL_TX_START) {
            byte *payload = (byte *)malloc(SERIAL_RX_BUFFER_SIZE + 1);
            int len = receive(payload);
            if(len == 0) {
                break;
            }
            payload = (byte *)realloc(payload, len + 1); // free some heap
            dispatch((Event *)newSerialEvent(SERIAL_RX_EVENT, payload));
            break;
        }
    }
}

byte receive(byte *rx) {
    // read from Serial and validate the transmission
    byte len = Serial.readBytesUntil(SERIAL_TX_END, rx, SERIAL_RX_BUFFER_SIZE);
    rx[len] = 0; // add null terminator
    if(rx[1] != SERIAL_PAYLOAD_START || rx[len - 1] != SERIAL_PAYLOAD_END) {
        free(rx);
        return 0;
    }

    // tokenize the transmission
    byte cksum = rx[0];
    strtok((char *)rx, SERIAL_PAYLOAD_START_S);
    char *payload = strtok(NULL, SERIAL_PAYLOAD_END_S);
    byte payloadLen = strlen(payload);
    
    // validate the checksum
    if(!crcVerify(cksum, (byte *)payload, payloadLen)) {
        free(rx);
        return 0;
    }

    // rearrange the bytes so that the buffer contains only the payload
    memmove(rx, payload, payloadLen + 1);
    return payloadLen;
}

SerialEvent *newSerialEvent(EventID eventID, byte *data) {
    SerialEvent *e = (SerialEvent *)malloc(sizeof(SerialEvent));
    e->event = {
        .id = eventID,
        .type = SERIAL_EVENT_TYPE,
        .ts = millis(),
        .destroy = _destroySerialEvent
    };
    e->data = data;
    return e;
}

void _destroySerialEvent(Event *e) {
    SerialEvent *evt = (SerialEvent *)e;
    free(evt->data);
    free(e);
}
*/