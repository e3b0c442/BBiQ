#include "button.h"
#include "event.h"
#include "ui.h"
#include "probe.h"

ScreenID uiDisplayScreen = SCREEN_INTRO;
ProbeID uiSelectedProbe = PROBE_0;
ProbeFieldID uiSelectedProbeField = PROBE_FIELD_NAME;
bool uiScreenSleeping = false;
bool displayPowered = true;
bool fieldEditing = false;

void selectPrevField();
void selectNextField();
void selectPrevProbe();
void selectNextProbe();
void uiEventHandler(Event *e);
LocalInputEvent* newLocalInputEvent(ProbeID probe, ProbeFieldID field, char delta);
void _destroyLocalInputEvent(Event *e);

void uiSetup() {
    registerHandler(BUTTON_DOWN_EVENT, &uiEventHandler);
    registerHandler(PROBE_DISCONNECT_EVENT, &uiEventHandler);
}

void selectPrevField() {
    do {
        if(uiSelectedProbeField == 0) {
            uiSelectedProbeField = (ProbeFieldID)((byte)PROBE_FIELD_COUNT - 1);
        } else {
            uiSelectedProbeField = (ProbeFieldID)((byte)uiSelectedProbeField - 1);
        }
    } while(!probeScreenFields[uiSelectedProbeField].rw);
}

void selectNextField() {
    do {
        if(uiSelectedProbeField == PROBE_FIELD_COUNT - 1) {
            uiSelectedProbeField = (ProbeFieldID)0;
        } else {
            uiSelectedProbeField = (ProbeFieldID)((byte)uiSelectedProbeField + 1);
        }
    } while(!probeScreenFields[uiSelectedProbeField].rw);
}

void selectPrevProbe() {
    do {
        if(uiSelectedProbe == 0) {
            uiSelectedProbe = (ProbeID)((byte)PROBE_COUNT - 1);
        } else {
            uiSelectedProbe = (ProbeID)((byte)uiSelectedProbe - 1);
        }
    } while(!probes[uiSelectedProbe].connected);
}

void selectNextProbe() {
    do {
        if(uiSelectedProbe == PROBE_COUNT - 1) {
            uiSelectedProbe = (ProbeID)0;
        } else {
            uiSelectedProbe = (ProbeID)((byte)uiSelectedProbe + 1);
        }
    } while(!probes[uiSelectedProbe].connected);
}

void uiEventHandler(Event *e) {
    switch(e->id) {
        case BUTTON_DOWN_EVENT:
            if(uiScreenSleeping) {
                return;
            }
            ButtonEvent *be = (ButtonEvent*)e;
            if(fieldEditing) {
                switch(be->button) {
                    case BUTTON_0:
                        fieldEditing = !fieldEditing;
                        break;
                    case BUTTON_1:
                        if(uiSelectedProbeField == PROBE_FIELD_NAME) {
                            selectPrevProbe();
                            break;
                        }
                    break;
                    case BUTTON_2:
                        if(uiSelectedProbeField == PROBE_FIELD_NAME) {
                            selectNextProbe();
                            break;
                        }
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

LocalInputEvent* newLocalInputEvent(ProbeID probe, ProbeFieldID field, char delta) {
    LocalInputEvent *e = (LocalInputEvent *)malloc(sizeof(LocalInputEvent));
    e->event = {
        .id = LOCAL_INPUT_EVENT,
        .type = LOCAL_INPUT_EVENT_TYPE,
        .ts = millis(),
        .destray = _destroyLocalInputEvent
    };
    e->probe = probe;
    e->field = field;
    e->delta = delta;
    return e;
}

void _destroyLocalInputEvent(Event *e) {
    free(e);
}