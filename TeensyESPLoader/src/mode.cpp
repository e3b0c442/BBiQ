#include <Arduino.h>
#include "event.h"
#include "mode.h"
#include "reset.h"

namespace
{
const uint32_t BOOT_DURATION = 75;
RunMode currentMode = RunMode::BOOT;
RunMode nextMode = RunMode::NORMAL;
uint32_t resetTime = 0;

void eventHandler(Event *e)
{
    switch (e->type)
    {
    case Event::Type::RESET:
        ResetEvent *re = (ResetEvent *)e;
        nextMode = re->mode;
        resetTime = re->ts;

        dispatch(new ModeEvent(RunMode::BOOT));
        break;
    }
}
} // namespace

void modeSetup()
{
    registerHandler(Event::Type::RESET, &eventHandler);
}

void modeLoop()
{
    uint32_t ts = millis();
    if (currentMode == RunMode::BOOT && int32_t(ts) - int32_t(BOOT_DURATION) > int32_t(resetTime))
    {
        currentMode = nextMode;
        dispatch(new ModeEvent(nextMode));
    }
}