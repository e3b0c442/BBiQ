#pragma once

#include <Arduino.h>
#include "mode.h"

struct ResetEvent : Event
{
    RunMode mode;
    ResetEvent(RunMode aMode) { type = Type::RESET, mode = aMode; };
};

void resetSetup();
void resetLoop(uint32_t ts);