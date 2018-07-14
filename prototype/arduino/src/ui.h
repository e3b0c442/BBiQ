#ifndef UI_E3B0C442_H
#define UI_E3B0C442_H

#include "probe.h"

typedef enum {
    SCREEN_INTRO,
    SCREEN_NOCONNECT,
    SCREEN_PROBE
} ScreenID;

typedef struct {
    ProbeFieldID id;
    bool rw;
    byte x, y, w;
    const char *pre, *post;
} ProbeScreenField;

typedef struct {
    Event event;
    ProbeID probe;
    ProbeFieldID field;
    char delta;
} LocalInputEvent;

const ProbeScreenField probeScreenFields[] = {
    {
        .id = PROBE_FIELD_NAME,
        .rw = true,
        .x = 0,
        .y = 0,
        .w = 10,
        .pre = "",
        .post = ""
    },
    {
        .id = PROBE_FIELD_TEMP,
        .rw = false,
        .x = 11,
        .y = 0,
        .w = 4,
        .pre = "",
        .post = "\xDF"
    },
    {
        .id = PROBE_FIELD_LOW,
        .rw = true,
        .x = 0,
        .y = 1,
        .w = 7,
        .pre = "L:",
        .post = ""
    },
    {
        .id = PROBE_FIELD_HIGH,
        .rw = true,
        .x = 8,
        .y = 1,
        .w = 7,
        .pre = "H:",
        .post = ""
    }
};

extern ScreenID uiDisplayScreen;
extern ProbeID uiSelectedProbe;
extern ProbeFieldID uiSelectedProbeField;
extern bool fieldEditing;
extern bool displayPowered;

void uiSetup();
void uiLoop();

#endif // UI_E3B0C442_H