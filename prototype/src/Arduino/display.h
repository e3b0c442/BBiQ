#ifndef DISPLAY_E3B0C442_H
#define DISPLAY_E3B0C442_H

#include <Arduino.h>
#include <LiquidCrystal.h>
#include "pin.h"
#include "probe.h"

typedef enum DisplayField {
    DISPLAY_FIELD_NAME = 0,
    DISPLAY_FIELD_TEMP = 1,
    DISPLAY_FIELD_LOW = 2,
    DISPLAY_FIELD_HIGH = 3
} DisplayField;

typedef struct {
    int x;
    int y;
    int w;
} DisplayFieldDef;

const unsigned int DISPLAY_FLASH_INTERVAL = 500;
const unsigned int DISPLAY_INTRO_LENGTH = 5000;

extern bool DISPLAY_INTRO_SCREEN;
extern bool DISPLAY_EDITING;
extern bool DISPLAY_FLASH_ON;
extern unsigned long DISPLAY_FLASH_TIME; 
extern bool DISPLAY_DIRTY;

const DisplayFieldDef DISPLAY_FIELDS[] = {
    { 0, 0, 10 },
    { 11, 0, 4 },
    { 0, 1, 7 },
    { 9, 1, 7 }
};

const int DISPLAY_EDITABLE_FIELD_COUNT = 3;
const DisplayField DISPLAY_EDITABLE_FIELDS[] = {
    DISPLAY_FIELD_NAME,
    DISPLAY_FIELD_LOW,
    DISPLAY_FIELD_HIGH
};

extern int DISPLAY_SELECTED_FIELD_INDEX;
extern int DISPLAY_SELECTED_PROBE;

extern LiquidCrystal LCD;

void displaySetup();
void displayLoop(Probe** probes, int len);
void drawProbeDisplay(Probe* probe);
void displayHandleEvent(Event* e);

#endif // DISPLAY_E3B0C442_H