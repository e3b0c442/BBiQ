#include "view_ctrl.h"
#include "button.h"
#include "event.h"
#include "probe.h"
#include "view_fields.h"

byte viewCtrlSelectedProbeIndex = 0;
bool viewCtrlEditing = false;

const byte viewCtrlSelectableFields[] = {
    VIEW_NAME_FIELD,
    VIEW_LOW_FIELD,
    VIEW_HIGH_FIELD
};
const byte viewCtrlSelectableFieldCount = 3;
byte viewCtrlSelectedFieldIndex = 0;


void viewCtrlEventHandler(Event* e);
ViewCtrlEvent* newViewCtrlEvent(byte field, char* value, bool selected, bool blank);
void _destroyViewCtrlEvent(Event* e);

void viewCtrlSetup() {
    registerHandler(PROBE_CHANGE_EVENT, &viewCtrlEventHandler);
    registerHandler(PROBE_CONNECT_EVENT, &viewCtrlEventHandler);
    registerHandler(PROBE_DISCONNECT_EVENT, &viewCtrlEventHandler);
    registerHandler(BUTTON_DOWN_EVENT, &viewCtrlEventHandler);
}

void viewCtrlEventHandler(Event* e) {
    Serial.println(F("HANDLING VIEW EVENT"));
    switch(e->id) {
        case BUTTON_DOWN_EVENT:
            ButtonEvent* be = (ButtonEvent*)e;
            switch(be->button) {
                case BUTTON_0:
                break;
                case BUTTON_1:
                    if(viewCtrlEditing) {

                    } else {
                        dispatch((Event*)newViewCtrlEvent(
                            viewCtrlSelectableFields[viewCtrlSelectedFieldIndex],
                            NULL,
                            false,
                            false
                        ));
                        if(viewCtrlSelectedFieldIndex == 0) {
                            viewCtrlSelectedFieldIndex = viewCtrlSelectableFieldCount - 1;
                        } else {
                            viewCtrlSelectedFieldIndex--;
                        }
                        dispatch((Event*)newViewCtrlEvent(
                            viewCtrlSelectableFields[viewCtrlSelectedFieldIndex],
                            NULL,
                            true,
                            false
                        ));
                    }
                break;
                case BUTTON_2:
                    if(viewCtrlEditing) {

                    } else {
                        dispatch((Event*)newViewCtrlEvent(
                            viewCtrlSelectableFields[viewCtrlSelectedFieldIndex],
                            NULL,
                            false,
                            false
                        ));
                        if(viewCtrlSelectedFieldIndex == viewCtrlSelectableFieldCount - 1) {
                            viewCtrlSelectedFieldIndex = 0;
                        } else {
                            viewCtrlSelectedFieldIndex++;
                        }
                        dispatch((Event*)newViewCtrlEvent(
                            viewCtrlSelectableFields[viewCtrlSelectedFieldIndex],
                            NULL,
                            true,
                            false
                        ));
                    }
                break;
            }
        break;
        case PROBE_CHANGE_EVENT:
        break;
    }
}

ViewCtrlEvent* newViewCtrlEvent(byte field, char* value, bool selected, bool blank) {
    Serial.println("Creating new ViewCtrlEvent");
    ViewCtrlEvent* e = (ViewCtrlEvent*) malloc(sizeof(ViewCtrlEvent));
    e->event = {
        VIEW_UPDATE_EVENT,
        VIEW_CTRL_EVENT_TYPE,
        millis(),
        _destroyViewCtrlEvent
    };
    e->field = field;
    e->value = value;
    e->selected = selected;
    e->blank = blank;
    return e;
}

void _destroyViewCtrlEvent(Event* e) {
    ViewCtrlEvent* vce = (ViewCtrlEvent*)e;
    if(vce->value != NULL) {
        free(vce->value);
    }
    free(e);
}