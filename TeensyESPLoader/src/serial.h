/**
 * serial.h
 * Copyright (c) 2019 Nick Meyer. All rights reserved.
 */

#ifndef SERIAL_E3B0C442_H
#define SERIAL_E3B0C442_H

#include "event.h"

#define SERIAL_BUFFER_SIZE 256

class SerialEvent : Event
{
  private:
    byte cmd;
    byte count;
    byte *data;
};

void serialSetup();
void serialReset();
void serialLoop();

#endif //SERIAL_E3B0C442_H