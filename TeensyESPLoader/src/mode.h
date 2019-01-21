#pragma once

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
};

void runModeSetup();
void runModeLoop();