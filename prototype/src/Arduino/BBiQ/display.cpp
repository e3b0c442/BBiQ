#include <LiquidCrystal.h>
#include <stdlib.h>
#include "display.h"
#include "event.h"
#include "pin.h"
#include "ui.h"
#include "version.h"

const unsigned long DISPLAY_TIMEOUT = 10000UL;
const unsigned long DISPLAY_FLASH_INTERVAL = 500UL;

LiquidCrystal LCD(    
    PIN_LCD_RS, 
    PIN_LCD_ENABLE, 
    PIN_LCD_D4, 
    PIN_LCD_D5,
    PIN_LCD_D6,
    PIN_LCD_D7
);

bool displayDirty = false;
unsigned long displayLastInteraction = 0UL;
bool displayFlash = true;
unsigned long displayLastFlash = 0UL;

void displayDraw();
void drawScreenIntro();
void drawScreenNoConnect();
void drawScreenProbe(ProbeID probeID);
void drawScreenProbeField(ProbeID probeID, ProbeFieldID fieldID);
void displayPowerOn();
void displayPowerOff();
void displayEventHandler(Event *e);
char *getProbeField(ProbeID probeID, ProbeFieldID fieldID);

void displaySetup() {
    pinMode(PIN_LCD_BKLT, OUTPUT);
    digitalWrite(PIN_LCD_BKLT, HIGH);
    LCD.begin(16, 2);
    displayDraw();
    registerHandler(BUTTON_DOWN_EVENT, &displayEventHandler);
    registerHandler(LOCAL_INPUT_EVENT, &displayEventHandler);
    registerHandler(PROBE_CHANGE_EVENT, &displayEventHandler);
}

void displayLoop() {
    if(fieldEditing) {
        unsigned long ts = millis();
        if((long)ts - (long)displayLastFlash > (long)DISPLAY_FLASH_INTERVAL) {
            displayLastFlash = ts;
            displayFlash = !displayFlash;
            displayDirty = true;
        }
    } else {
        if(!displayFlash) {
            displayFlash = true;
            displayDirty = true;
        }
    }

    if(displayDirty) {
        displayDraw();
        displayDirty = false;
    }
}

void displayDraw() {
    switch(uiDisplayScreen) {
        case SCREEN_INTRO:
            drawScreenIntro();
            break;
        case SCREEN_NOCONNECT:
            drawScreenNoConnect();
            break;
        case SCREEN_PROBE:
            drawScreenProbe(uiSelectedProbe);
            break;
    }
}

void drawScreenIntro() {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print(F("BBiQ v"));
    LCD.print(BBIQ_VERSION);
    LCD.setCursor(0, 1);
    LCD.print(F("Smoke on"));
}

void drawScreenNoConnect() {
    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print(F("No probes found"));
}

void drawScreenProbe(ProbeID probeID) {
    for(byte i = 0; i < PROBE_FIELD_COUNT; i++) {
        drawScreenProbeField(probeID, (ProbeFieldID)i);
    }
}

void drawScreenProbeField(ProbeID probeID, ProbeFieldID fieldID) {
    const ProbeScreenField *field = &probeScreenFields[fieldID];
    LCD.setCursor(field->x, field->y);
    LCD.print(field->pre);

    if(field->rw && fieldID == uiSelectedProbeField) {
        LCD.print(F("["));
    } else {
        LCD.print(F(" "));
    }

    byte maxLen = field->w - 2 - strlen(field->pre) - strlen(field->post);
    char toPrint[maxLen + 1];
    toPrint[maxLen] = '\0';
    memset(toPrint, ' ', maxLen);
    if(displayFlash) {
        char *val = getProbeField(probeID, fieldID);
        byte valLen = strlen(val);
        if(valLen >= maxLen) {
            strncpy(toPrint, val, valLen);
        } else {
            strcpy(toPrint, val);
        }
        free(val);
    }
    LCD.print(toPrint);

    if(field->rw && fieldID == uiSelectedProbeField) {
        LCD.print(F("]"));
    } else {
        LCD.print(F(" "));
    }

    LCD.print(field->post);
}

void displayPowerOn() {
    displayLastInteraction = millis();
    if(!displayPowered) {
        LCD.display();
        digitalWrite(PIN_LCD_BKLT, HIGH);
        displayPowered = true;
    }
}

void displayPowerOff() {
    if(displayPowered) {
        LCD.noDisplay();
        digitalWrite(PIN_LCD_BKLT, LOW);
        displayPowered = false;
    }
}

void displayEventHandler(Event *e) {
    switch(e->id) {
        case BUTTON_DOWN_EVENT:
            displayPowerOn();
            break;
        default:
            displayDirty = true;
            break;
    }
}

char *getProbeField(ProbeID probeID, ProbeFieldID fieldID) {
    Probe *probe = &probes[probeID];
    char *r = (char*) malloc(13 * sizeof(char));
    switch(fieldID) {
        case PROBE_FIELD_NAME:
            strcpy(r, probe->name);
            break;
        case PROBE_FIELD_TEMP:
            itoa((int)(probe->temperature + 0.5), r, 10);
            break;
        case PROBE_FIELD_LOW:
            itoa((int)(probe->lowAlarm + 0.5), r, 10);
            break;
        case PROBE_FIELD_HIGH:
            itoa((int)(probe->highAlarm + 0.5), r, 10);
            break;
    }
    return r;
}