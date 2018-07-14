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

#define SERIAL_BAUD_RATE 9600

typedef struct {
    Event event;
    byte *data;
} SerialEvent;

void serialSetup();
void serialLoop();

#endif //SERIAL_E3B0C442_H
