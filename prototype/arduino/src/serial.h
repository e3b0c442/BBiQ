/**
 * serial.h
 * Copyright (c) 2018 Nick Meyer. All rights reserved.
 * 
 * Handles serial IO between the MCU and connected device via the Firmata
 * protocol. Since we are not doing standard device control, all transmissions
 * will use the Sysex message extended command set to send and receive
 * arbitrary bytes; in effect, Firmata is mainly used to handle serial
 * transmission and receipt, but this gives us the option to easily utilize
 * the standard Firmata command types in the future if needed.
 * 
 * The standard format of the sysex message is as follows:
 * byte     value
 * 0        SOH
 * 1        8-bit CRC of everything between STX and ETX exclusive
 * 2        STX
 * 3..30    Data (up to 28 bytes, no padding required)
 * DAT+1    ETX
 * ETX+1    EOT
 * 
 */

#ifndef SERIAL_E3B0C442_H
#define SERIAL_E3B0C442_H

#include <SoftwareSerial.h>
#include "event.h"

typedef struct {
    Event event;
    byte cmd;
    byte count;
    byte *data;
} SerialEvent;

void serialSetup();
void serialLoop();

#endif //SERIAL_E3B0C442_H
