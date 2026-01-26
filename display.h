/**
 * ============================================================================
 * DISPLAY.H - LCD Display Configuration and Interface
 * ============================================================================
 * 
 * Configures the 16x2 LCD display connected via I2C interface.
 * Provides global LCD object and timing utilities for display management.
 * 
 * Hardware Connections:
 *   GND   - Black wire
 *   VCC   - Red wire (5V)
 *   SDA   - White wire (I2C Data)
 *   SCL   - Gray wire (I2C Clock)
 * 
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <LiquidCrystal_I2C.h>
#include <stdint.h>

// LCD I2C address (0x27 is default for 16x2 displays)
#define SCREEN_LOCATION 0x27
// LCD dimensions
#define SCREEN_WIDTH 16
#define SCREEN_HEIGHT 2

// Global LCD object for display operations
extern LiquidCrystal_I2C lcd;
// Timer for display dimming functionality
extern uint32_t dimTimer;

#endif