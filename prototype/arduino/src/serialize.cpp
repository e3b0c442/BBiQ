#include "serialize.h"
#include "serial.h"

#define SYSTEM_STARTUP_MESSAGE      0x00
#define PROBE_MESSAGE               0x01

void serializeProbeEvent(byte probeID, byte *buf, byte count) {
    buf[0] = STX;
    buf[1] = PROBE_MESSAGE;
    buf[2] = US;
    buf[3] = probeID;
    buf[4] = US;
    buf[5] = value;
    buf[9] = ETX;
}