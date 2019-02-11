#include <Arduino.h>

#include "buzzer.hpp"
#include "event.hpp"
#include "pins.hpp"
#include "probe.hpp"

namespace
{
enum class ToneDuration : uint8_t
{
    SHORT,
    LONG
};

const uint16_t BUZZER_FREQUENCY = 2700;
const uint8_t BEEP_SEQUENCE = 4;
const uint32_t BEEP_LENGTH = 100;
const uint32_t BEEP_SHORT_INTERVAL = 200;
const uint32_t BEEP_LONG_INTERVAL = 1000;

uint8_t beepCount = 0;
ToneDuration nextInterval = ToneDuration::SHORT;
uint32_t lastBeep = 0;

bool buzzerAlarming = false;

void handler(Event &e)
{
    switch (e.type)
    {
    case Event::Type::BUTTON:
        buzzerAlarming = false;
        break;
    case Event::Type::PROBE:
        ProbeEvent &pe = (ProbeEvent &)e;
        if (pe.action == ProbeEvent::Action::ALARM)
            buzzerAlarming = true;
        break;
    }
}

} // namespace

void buzzerSetup()
{
    pinMode((uint8_t)Pin::BUZZER, OUTPUT);
    registerHandler(Event::Type::PROBE, &handler);
    registerHandler(Event::Type::BUTTON, &handler);
}

void buzzerLoop(int32_t ts)
{
    if (buzzerAlarming)
    {
        switch (nextInterval)
        {
        case ToneDuration::SHORT:
            if ((long)ts - (long)lastBeep < (long)BEEP_SHORT_INTERVAL)
                return;
            break;
        case ToneDuration::LONG:
            if ((long)ts - (long)lastBeep < (long)BEEP_LONG_INTERVAL)
                return;
            nextInterval = ToneDuration::SHORT;
            break;
        }
        lastBeep = ts;
        beepCount++;
        if (beepCount >= BEEP_SEQUENCE)
        {
            beepCount = 0;
            nextInterval = ToneDuration::LONG;
        }
        tone((uint8_t)Pin::BUZZER, BUZZER_FREQUENCY, BEEP_LENGTH);
    }
}