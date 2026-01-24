#include "doomtype.h"

// PrBoom expects these globals
int usejoystick = 0;
int joyleft     = 0;
int joyright    = 0;
int joyup       = 0;
int joydown     = 0;

// Stub for jsInit() called in app_main
void jsInit(void) {
    // nothing yet, joystick not implemented
}
