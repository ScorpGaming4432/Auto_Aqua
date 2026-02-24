/**
 * ============================================================================
 * INPUT.CPP - Input Handling Implementation
 * ============================================================================
 *
 * Processes keypad input and dispatches to appropriate handlers.
 */

#include "input.h"
#include "appstate.h"
#include "screens.h"
#include "storage.h"
#include "water.h"
#include "language.h"
#include "display.h"
#include <Keypad.h>
#include <Arduino.h>

extern Keypad keypad;
extern LiquidCrystal_I2C lcd;
