#pragma once

#include <Arduino.h>
#include "event.hpp"
#include "mode.hpp"

struct ResetEvent : Event
{
    RunMode mode;

    ResetEvent(RunMode aMode) { type = Event::Type::RESET, mode = aMode; };
};

void reset(RunMode mode);
void resetSetup();