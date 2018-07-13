#ifndef UI_E3B0C442_H
#define UI_E3B0C442_H

#include "probe.h"

typedef enum {
    SCREEN_INTRO,
    SCREEN_NOCONNECT,
    SCREEN_PROBE
} ScreenID;

typedef enum {
    PROBE_FIELD_NAME,
    PROBE_FIELD_TEMP,
    PROBE_FIELD_LOW,
    PROBE_FIELD_HIGH,
    PROBE_FIELD_COUNT
} ProbeFieldID;

typedef struct {
    ProbeFieldID id;
    bool rw;
    byte x, y, w;
    const char *pre, *post;
} ProbeScreenField;

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
        .x = 12,
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

#endif // UI_E3B0C442_H