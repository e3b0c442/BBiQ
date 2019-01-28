#include <Arduino.h>
#include "button.hpp"
#include "event.hpp"
#include "mode.hpp"
#include "probe.hpp"
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
    switch (e->type)
    {
    case Event::Type::RESET:
        Serial.printf("DEBUG: [%10u] Received reset event\n", e->ts);
        break;
    case Event::Type::MODE:
    {
        ModeEvent *me = (ModeEvent *)e;
        Serial.printf("DEBUG: [%10u] Received mode event (mode %d)\n", e->ts, me->mode);
        break;
    }
    case Event::Type::BUTTON:
    {
        ButtonEvent *be = (ButtonEvent *)e;
        Serial.printf("DEBUG: [%10u] Received button event (state %d)\n", e->ts, be->state);
        break;
    }
    case Event::Type::PROBE:
    {
        ProbeEvent *pe = (ProbeEvent *)e;
        Serial.printf("DEBUG: [%10u] Received probe event (probe %d, connected %d, temp %f\n", e->ts, pe->probe->id, pe->probe->connected, pe->probe->temperature);
        break;
    }
    default:;
    }
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
            Serial3.begin(ESP_NORMAL_BAUD_RATE);
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
    registerHandler(Event::Type::BUTTON, &handler);
    registerHandler(Event::Type::PROBE, &handler);
#endif
}

void serialLoop(uint32_t *ts __attribute__((unused)))
{
    switch (currentMode)
    {
    case RunMode::NORMAL:
        while (Serial3.available())
            Serial.write(Serial3.read());
    //fallthrough
    case RunMode::PROGRAM:
    case RunMode::BOOT:
        while (Serial1.available())
            Serial.write(Serial1.read());
        while (Serial.available())
            Serial1.write(Serial.read());
    }
}