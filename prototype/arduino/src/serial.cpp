#include <Firmata.h>
#include "crc.h"
#include "event.h"
#include "probe.h"
#include "serial.h"
#include "serialize.h"

#define SYSEX_BBIQ_MESSAGE 0x01

void sysexCallback(byte cmd, byte count, byte* data);
SerialEvent *newSerialEvent(byte cmd, byte count, byte *data);
void _destroySerialEvent(Event *e);
void serialEventHandler(Event *e);

void serialSetup() {
    registerHandler(PROBE_CHANGE_EVENT, &serialEventHandler);
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

void serialEventHandler(Event *e){
    switch(e->type) {
        case PROBE_CHANGE_EVENT:
            ProbeEvent *pce = (ProbeEvent *)e;
            ProbeID probe = pce->probe;
            float temp = probes[probe].
            break;
        default:
    }
}