/**
 * serial.h
 * 
 * Handles serial IO to and from the MCU, including adding the CRC header to
 * outgoing packets and checking the CRC of incoming packets.
 * 
 * All serial transmissions start with an ASCII SOH (0x01) character. 
 * Immediately after the SOH character there is a 2-digit 8-bit CRC in 
 * hexadecimal format. This is followed by ASCII STX (0x02) which indicates the
 * beginning of the payload. The payload ends with the ASCII ETX (0x03) and
 * finally ASCII EOT (0x04) to signify the end of the transmission. Any
 * transmission that is not bounded by SOH/EOT or whose checksum does not match
 * is discarded. The checksummed payload does not include the delimiters.
 */
#ifndef SERIAL_E3B0C442_H
#define SERIAL_E3B0C442_H

#include "event.h"

#define SERIAL_BAUD_RATE        9600
#define SERIAL_TX_START         0x01
#define SERIAL_TX_END           0x04
#define SERIAL_PAYLOAD_START    0x02
#define SERIAL_PAYLOAD_END      0x03
#define SERIAL_PAYLOAD_START_S  "\x02"
#define SERIAL_PAYLOAD_END_S    "\x03"
#define SERIAL_BUFFER_LEN       64

typedef struct _SerialEvent {
    Event event;
    byte* data;
} SerialEvent;

void serialSetup();
void serialLoop();
byte receive(byte* payload);
SerialEvent* newSerialEvent(byte eventID, byte* data);
void _destroySerialEvent(Event* evt);

#endif //SERIAL_E3B0C442_H
