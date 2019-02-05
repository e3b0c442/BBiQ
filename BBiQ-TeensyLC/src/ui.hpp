#pragma once

#include <Arduino.h>
#include "event.hpp"
#include "probe.hpp"

enum class Screen : uint8_t
{
    SPLASH,
    PROGRAM,
    PROBE,
    COUNT
};

struct UIEvent : Event
{
    bool powerSave;
    Screen screen;
    Probe *probe;

    UIEvent(Screen sc, bool ps, Probe *pr) : powerSave(ps), screen(sc), probe(pr) { type = Event::Type::UI; };

#ifdef DEBUG
    void log(Stream &);
#endif
};

void uiSetup();
void uiLoop(uint32_t ts);
