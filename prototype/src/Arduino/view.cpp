#include <string.h>
#include <LiquidCrystal.h>
#include "event.h"
#include "pin.h"
#include "version.h"
#include "view.h"

LiquidCrystal LCD(    
    PIN_LCD_RS, 
    PIN_LCD_ENABLE, 
    PIN_LCD_D4, 
    PIN_LCD_D5,
    PIN_LCD_D6,
    PIN_LCD_D7
);

typedef struct {
    int x;
    int y;
    int width;
    char* prefix;
    char* value;
    char* postfix;
    bool selectable;
    bool selected;
    bool blank;
} ViewField;

const unsigned long VIEW_INTRO_DURATION = 5000UL;
const unsigned long VIEW_SLEEP_TIMER = 10000UL;
bool viewIntro = true;
bool viewDirty = false;
bool viewActive = true;
const unsigned long lastInteraction = 0UL;

ViewField viewNameField = { 0, 0, 10, "", "", "", true, true, false };
ViewField viewTempField = { 12, 0, 4, "", "", "\xdf", false, false, false };
ViewField viewLowField = { 0, 1, 7, "L:", "", "", true, false, false };
ViewField viewHighField = { 0, 1, 7, "H:", "", "", true, false, false };

void _drawField(ViewField* field);
void _powerOnDisplay();
void _powerOffDisplay();

void viewSetup() {
    _powerOnDisplay();
    LCD.begin(16, 2);
    LCD.setCursor(0, 0);
    LCD.print(F("BBiQ v"));
    LCD.print(BBIQ_VERSION);
    LCD.setCursor(0, 1);
    LCD.print(F("Smoke on"));
}

void viewLoop() {
    unsigned long ts = millis();
    if(viewIntro) {
        if((long)ts - long(VIEW_INTRO_DURATION) > 0) {
            viewIntro = false;
            viewDirty = true;
            LCD.clear();
        }
    }
    
    if(viewDirty) {
        _drawField(&viewNameField);
        _drawField(&viewTempField);
        _drawField(&viewLowField);
        _drawField(&viewHighField);
    }
}

void _drawField(ViewField* field) {
    LCD.setCursor(field->x, field->y);
    LCD.print(field->prefix);

    if(field->selectable) {
        if(field->selected) {
            LCD.print(F("["));
        } else {
            LCD.print(F(" "));
        }
    }
    
    byte valueLen = field->width - 2 - strlen(field->prefix) - strlen(field->postfix);
    char toPrint[valueLen + 1];
    toPrint[valueLen] = '\0';
    if(field->blank) {
        memset(toPrint, ' ', valueLen);
    } else {
        byte fieldLen = strlen(field->value);
        if(fieldLen >= valueLen) {
            strncpy(toPrint, field->value, valueLen);
        } else {
            strcpy(toPrint, field->value);
            memset(toPrint + fieldLen, ' ', valueLen - fieldLen);
        }
    }
    LCD.print(toPrint);

    if(field->selectable) {
        if(field->selected) {
            LCD.print(F("]"));
        } else {
            LCD.print(F(" "));
        }
    }

    LCD.print(field->postfix);
}

void _powerOnDisplay() {
    LCD.display();
    digitalWrite(PIN_LCD_BKLT, HIGH);
}

void _powerOffDisplay() {
    LCD.noDisplay();
    digitalWrite(PIN_LCD_BKLT, LOW);
}

void viewEventHandler(Event* e) {
    switch(e->type) {
        case BUTTON_EVENT_TYPE:
            _powerOnDisplay();
        break;
    };
    e->destroy;
}