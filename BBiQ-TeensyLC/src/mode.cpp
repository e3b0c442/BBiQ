#include <Arduino.h>
#include "event.hpp"
#include "mode.hpp"
#include "reset.hpp"

namespace
{
const uint32_t BOOT_DURATION = 75;

bool booting = false;
uint32_t bootTime = 0;
RunMode nextMode = RunMode::NORMAL;

void handler(Event &e)
{
    switch (e.type)
    {
    case Event::Type::RESET:
    {
        ResetEvent &re = (ResetEvent &)e;
        booting = true;
        bootTime = re.ts;
        nextMode = re.mode;
        dispatch(new ModeEvent(RunMode::BOOT));
        break;
    }
    default:;
    }
}

} // namespace

void modeSetup()
{
    registerHandler(Event::Type::RESET, &handler);
}

void modeLoop(uint32_t ts)
{
    if (booting && (int32_t)ts - (int32_t)bootTime > (int32_t)BOOT_DURATION)
    {
        booting = false;
        dispatch(new ModeEvent(nextMode));
    }
}

#ifdef DEBUG
void ModeEvent::log(Stream &s)
{
    const char *modes[] = {
        "BOOT",
        "PROGRAM",
        "NORMAL",
    };

    Event::prelog(s);
    s.printf("TYPE: MODE; MODE: %s\n", modes[(uint8_t)mode]);
}
#endif // DEBUG