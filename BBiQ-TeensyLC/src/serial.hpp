#pragma once

#include <Arduino.h>

#define UART_BUFFER_SIZE 256

struct SerialEvent : Event
{
    char *buffer;
    uint8_t len;

    SerialEvent(char *b, uint8_t l) : buffer(b), len(l) { type = Event::Type::UART; };
};

void serialSetup();
void serialLoop(uint32_t ts);