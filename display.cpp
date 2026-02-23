/**
 * ============================================================================
 * DISPLAY.CPP - LCD Display Implementation
 * ============================================================================
 *
 * Initializes and manages the 16x2 LCD display connected via I2C.
 */

#include "display.h"

// Create global LCD object with specified I2C address and dimensions
LiquidCrystal_I2C lcd(Hardware::LCD_I2C_ADDRESS, Hardware::LCD_WIDTH, Hardware::LCD_HEIGHT);
// Timer for display dimming/timeout functionality
uint32_t dimTimer = 0u;
