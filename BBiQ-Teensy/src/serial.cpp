#include <Arduino.h>
#include "event.hpp"
#include "mode.hpp"
#include "serial.hpp"

namespace
{
const uint32_t USB_SERIAL_BAUD_RATE = 115200;
const uint32_t ESP_BOOT_BAUD_RATE = 74480;
const uint32_t ESP_NORMAL_BAUD_RATE = 115200;

RunMode currentMode = RunMode::BOOT;

void logo() // because why not
{
    Serial.println("   (   (      (");
    Serial.println(" ( )\\( )\\(  ( )\\");
    Serial.println(" )((_)((_)\\ )((_)");
    Serial.println("((_)((_)((_|(_)_");
    Serial.println(" | _ ) _ |_)/ _ \\");
    Serial.println(" | _ \\ _ \\ | (_) |");
    Serial.println(" |___/___/_|\\__\\_\\");
    Serial.println("BBiQ v0.1 - 2019-01-22");
}

void handler(Event *e)
{
#ifdef DEBUG
    Serial.printf("DEBUG: Received event %d\n", e->type);
#endif
    switch (e->type)
    {
    case Event::Type::MODE:
    {
        ModeEvent *me = (ModeEvent *)e;
        currentMode = me->mode;

        switch (me->mode)
        {
        case RunMode::BOOT:
#ifdef DEBUG
            Serial.println("DEBUG: Mode change: BOOT");
#endif
            logo();
            Serial1.end();
            Serial1.begin(ESP_BOOT_BAUD_RATE);
            break;
        case RunMode::NORMAL:
#ifdef DEBUG
            Serial.println("DEBUG: Mode change: NORMAL");
#endif
            Serial1.end();
            Serial2.begin(ESP_NORMAL_BAUD_RATE);
            Serial1.begin(ESP_NORMAL_BAUD_RATE);
            break;
        case RunMode::PROGRAM:
#ifdef DEBUG
            Serial.println("DEBUG: Mode change: PROGRAM");
#endif
            Serial1.end();
            Serial1.begin(ESP_NORMAL_BAUD_RATE);
            break;
        }
    }
    default:;
    }
}
} // namespace

void serialSetup()
{
    uint32_t ts = millis();
    Serial.begin(USB_SERIAL_BAUD_RATE);
    while (!Serial && millis() - ts < 2500)
        ;
    Serial1.begin(ESP_NORMAL_BAUD_RATE);

    registerHandler(Event::Type::MODE, &handler);

#ifdef DEBUG
    registerHandler(Event::Type::RESET, &handler);
#endif
}

void serialLoop(uint32_t *ts __attribute__((unused)))
{
    switch (currentMode)
    {
    case RunMode::NORMAL:
        while (Serial2.available())
            Serial.write(Serial2.read());
    //fallthrough
    case RunMode::PROGRAM:
    case RunMode::BOOT:
        while (Serial1.available())
            Serial.write(Serial1.read());
        while (Serial.available())
            Serial1.write(Serial.read());
    }
}