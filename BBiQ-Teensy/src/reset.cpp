#include <Arduino.h>
#include "event.hpp"
#include "mode.hpp"
#include "pins.hpp"
#include "reset.hpp"

void reset(RunMode mode)
{
    Serial2.end();
    pinMode((uint8_t)Pin::ESP_IO2, OUTPUT);
    digitalWrite((uint8_t)Pin::ESP_IO2, HIGH);

    switch (mode)
    {
    case RunMode::NORMAL:
        digitalWrite((uint8_t)Pin::ESP_IO0, HIGH);
        break;
    case RunMode::PROGRAM:
        digitalWrite((uint8_t)Pin::ESP_IO0, LOW);
        break;
    default:;
    }

    digitalWrite((uint8_t)Pin::ESP_RST, LOW);
    delay(50);
    digitalWrite((uint8_t)Pin::ESP_RST, HIGH);

    dispatch(new ResetEvent(mode));
}

void resetSetup()
{
    pinMode((uint8_t)Pin::ESP_IO0, OUTPUT);
    pinMode((uint8_t)Pin::ESP_IO2, OUTPUT);
    pinMode((uint8_t)Pin::ESP_RST, OUTPUT);
    digitalWrite((uint8_t)Pin::ESP_RST, LOW);
}