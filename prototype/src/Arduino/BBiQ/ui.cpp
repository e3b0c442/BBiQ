#include "button.h"
#include "event.h"
#include "ui.h"
#include "probe.h"

ScreenID uiDisplayScreen = SCREEN_INTRO;
ProbeID uiSelectedProbe = PROBE_0;
ProbeFieldID uiSelectedProbeField = PROBE_FIELD_NAME;
bool uiScreenSleep = false;
bool displayPowered = true;
bool fieldEditing = false;

void selectPrevField();
void selectNextField();
void uiEventHandler(Event *e);

void uiSetup() {
    registerHandler(BUTTON_DOWN_EVENT, &uiEventHandler);
    registerHandler(PROBE_DISCONNECT_EVENT, &uiEventHandler);
}

void selectPrevField() {
    do {
        if(uiSelectedProbeField == 0) {
            uiSelectedProbeField = PROBE_FIELD_COUNT - 1;
        } else {
            uiSelectedProbeField = (byte)uiSelectedProbeField - 1;
        }
    } while(probeScreenFields[uiSelectedProbeField].rw != true);
}

void selectNextField() {
    do {
        if(uiSelectedProbeField == PROBE_FIELD_COUNT - 1) {
            uiSelectedProbeField = 0;
        } else {
            uiSelectedProbeField = (byte)uiSelectedProbeField + 1;
        }
    } while(probeScreenFields[uiSelectedProbeField].rw != true);
}

void uiEventHandler(Event *e) {
    switch(e->id) {
        case BUTTON_DOWN_EVENT:
            ButtonEvent *be = (ButtonEvent*)e;
            if(fieldEditing) {
                switch(be->button) {
                    case BUTTON_0:
                    break;
                    case BUTTON_1:
                    break;
                    case BUTTON_2:
                    break;
                }
            } else {
                switch(be->button) {
                    case BUTTON_0:
                        fieldEditing = !fieldEditing;
                        break;
                    case BUTTON_1:
                        selectPrevField();
                        break;
                    case BUTTON_2:
                        selectNextField();
                        break;
                }
            }
    }
}