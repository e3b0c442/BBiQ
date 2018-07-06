#include <LiquidCrystal.h>
#include "display.h"
#include "version.h"

bool DISPLAY_INTRO_SCREEN = true;
bool DISPLAY_EDITING = false;
bool DISPLAY_FLASH_ON = true;
unsigned long DISPLAY_FLASH_TIME = 0;
bool DISPLAY_DIRTY = true;
int DISPLAY_SELECTED_FIELD_INDEX = 0;
int DISPLAY_SELECTED_PROBE = 0;

LiquidCrystal LCD(    
    PIN_LCD_RS, 
    PIN_LCD_ENABLE, 
    PIN_LCD_D4, 
    PIN_LCD_D5,
    PIN_LCD_D6,
    PIN_LCD_D7
);

void displaySetup() {
    LCD.begin(16, 2);
    LCD.setCursor(0, 0);
    LCD.print("BBiQ v" + BBIQ_VERSION);
    LCD.setCursor(0, 1);
    LCD.print("Smoke on");
}

void displayLoop(Probe** probes, int len) {
    unsigned long loopTime = millis();
    if(DISPLAY_INTRO_SCREEN) {
        if(long(DISPLAY_INTRO_LENGTH) - long(loopTime) < 0) {
            DISPLAY_INTRO_SCREEN = false;
            drawProbeDisplay(probes[DISPLAY_SELECTED_PROBE]);
        }
        return;
    }

    if(DISPLAY_EDITING && loopTime - DISPLAY_FLASH_TIME > DISPLAY_FLASH_INTERVAL) {
        DISPLAY_FLASH_ON = !DISPLAY_FLASH_ON;
        DISPLAY_FLASH_TIME = loopTime;
        DISPLAY_DIRTY = true;
    }

    if(DISPLAY_DIRTY) {
        Serial.println("Display set dirty, about to draw");
        drawProbeDisplay(probes[DISPLAY_SELECTED_PROBE]);
        DISPLAY_DIRTY = false;
    }
}

void drawProbeDisplay(Probe* probe) {
    Serial.print("About to draw probe " + probe->name + " with temp ");
    Serial.println(probe->temperature);
    // Name field
    LCD.setCursor(DISPLAY_FIELDS[DISPLAY_FIELD_NAME].x, DISPLAY_FIELDS[DISPLAY_FIELD_NAME].y);
    if(DISPLAY_EDITABLE_FIELDS[DISPLAY_SELECTED_FIELD_INDEX] == DISPLAY_FIELD_NAME) {
        LCD.print("[");
        if(DISPLAY_EDITING && !DISPLAY_FLASH_ON) {
            for(int i = 0; i < DISPLAY_FIELDS[DISPLAY_FIELD_NAME].w - 2; i++) {
                LCD.print(" ");
            }
        } else {
            if(probe->name.length() > DISPLAY_FIELDS[DISPLAY_FIELD_NAME].w - 2) {
                LCD.print(probe->name.substring(0,DISPLAY_FIELDS[DISPLAY_FIELD_NAME].w - 2));
            } else {
                LCD.print(probe->name);
                for(int i = 0; i < DISPLAY_FIELDS[DISPLAY_FIELD_NAME].w - 2 - probe->name.length(); i++) {
                    LCD.print(" ");
                }
            }
        }
        LCD.print("]");
    } else {
        LCD.print(" ");
        if(probe->name.length() > DISPLAY_FIELDS[DISPLAY_FIELD_NAME].w - 2) {
            LCD.print(probe->name.substring(0,DISPLAY_FIELDS[DISPLAY_FIELD_NAME].w));
        } else {
            LCD.print(probe->name);
            for(int i = 0; i < DISPLAY_FIELDS[DISPLAY_FIELD_NAME].w - 2- probe->name.length(); i++) {
                LCD.print(" ");
            }
        }
        LCD.print(" ");
    }

    // Current temp field
    LCD.setCursor(DISPLAY_FIELDS[DISPLAY_FIELD_TEMP].x, DISPLAY_FIELDS[DISPLAY_FIELD_TEMP].y);
    int absTemp = abs(int(probe->temperature + 0.5));
    if(probe->temperature < 0) {
        LCD.print("-");
    } else {
        LCD.print(" ");
    }
    if(absTemp < 100) {
        LCD.print(" ");
        if(absTemp < 10) {
            LCD.print(" ");
        }
    }
    LCD.print(absTemp);
    LCD.print(char(0xDF));

    // Low alarm field
    LCD.setCursor(DISPLAY_FIELDS[DISPLAY_FIELD_LOW].x, DISPLAY_FIELDS[DISPLAY_FIELD_LOW].y);
    LCD.print("L:");
    if(DISPLAY_EDITABLE_FIELDS[DISPLAY_SELECTED_FIELD_INDEX] == DISPLAY_FIELD_LOW) {
        LCD.print("[");
        if(DISPLAY_EDITING && !DISPLAY_FLASH_ON) {
            LCD.print("   ");
        } else {
            if(probe->lowAlarm < 100) {
                LCD.print(" ");
                if(probe->lowAlarm < 10) {
                    LCD.print(" ");
                }
            }
            LCD.print(int(probe->lowAlarm));
        }
        LCD.print("]");
    } else {
        LCD.print(" ");
        if(probe->lowAlarm < 100) {
            LCD.print(" ");
            if(probe->lowAlarm < 10) {
                LCD.print(" ");
            }
        }
        LCD.print(int(probe->lowAlarm));
        LCD.print("  ");
    }

    // High alarm field
    LCD.setCursor(DISPLAY_FIELDS[DISPLAY_FIELD_HIGH].x, DISPLAY_FIELDS[DISPLAY_FIELD_HIGH].y);
    LCD.print("H:");
    if(DISPLAY_EDITABLE_FIELDS[DISPLAY_SELECTED_FIELD_INDEX] == DISPLAY_FIELD_HIGH) {
        LCD.print("[");
        if(DISPLAY_EDITING && !DISPLAY_FLASH_ON) {
            LCD.print("   ");
        } else {
            if(probe->highAlarm < 100) {
                LCD.print(" ");
                if(probe->highAlarm < 10) {
                    LCD.print(" ");
                }
            }
            LCD.print(int(probe->highAlarm));
        }
        LCD.print("]");    
    } else {
        LCD.print(" ");
        if(probe->highAlarm < 100) {
            LCD.print(" ");
            if(probe->highAlarm < 10) {
                LCD.print(" ");
            }
        }
        LCD.print(int(probe->highAlarm));
        LCD.print(" ");
    }
}


