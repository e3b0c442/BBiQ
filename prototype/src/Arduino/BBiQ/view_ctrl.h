#ifndef VIEW_CTRL_E3B0C442_H
#define VIEW_CTRL_E3B0C442_H

#include "event.h"

typedef struct {
    Event event;
    byte field;
    char* value;
    bool selected;
    bool blank;
} ViewCtrlEvent;

void viewCtrlSetup();
void viewCtrlLoop();

#endif // VIEW_CTRL_E3B0C442_H