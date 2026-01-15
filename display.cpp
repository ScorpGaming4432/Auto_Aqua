/**
 * ============================================================================
 * DISPLAY.CPP - LCD Display Implementation
 * ============================================================================
 * 
 * Initializes and manages the 16x2 LCD display connected via I2C.
 */

#include "display.h"

// Create global LCD object with specified I2C address and dimensions
extern LiquidCrystal_I2C lcd(SCREEN_LOCATION, SCREEN_WIDTH, SCREEN_HEIGHT);
// Timer for display dimming/timeout functionality
uint32_t dimTimer = 0u;
