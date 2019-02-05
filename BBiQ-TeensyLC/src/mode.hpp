#pragma once

#include <Arduino.h>
#include "event.hpp"

enum class RunMode
{
    BOOT,
    PROGRAM,
    NORMAL,
};

struct ModeEvent : Event
{
    RunMode mode;
    ModeEvent(RunMode aMode) { type = Event::Type::MODE, mode = aMode; };

#ifdef DEBUG
    void log(Stream &);
#endif // DEBUG
};

void modeSetup();
void modeLoop(uint32_t ts);