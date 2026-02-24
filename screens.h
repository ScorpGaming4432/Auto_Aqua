/**
 * ============================================================================
 * SCREENS.H - User Interface Screen Declarations
 * ============================================================================
 *
 * Defines all screen display functions and keypad interface for the aquarium
 * management system. Handles user input for configuration and monitoring.
 *
 * Keypad Layout (4x4 Matrix):
 *   1  2  3  A
 *   4  5  6  B
 *   7  8  9  C
 *   *  0  #  D
 *
*/

#ifndef SCREENS_H
#define SCREENS_H

#include <Keypad.h>
#include <stdint.h>

#include "hardware.h"
#include "language.h"

// Keypad configuration - 4x4 matrix
// inside hardware.h to avoid hardware leakage into screens.cpp

// Keypad layout definition
const char keys[Hardware::KEYPAD_ROWS][Hardware::KEYPAD_COLS] = { { '1', '2', '3', 'A' },
                                                                  { '4', '5', '6', 'B' },
                                                                  { '7', '8', '9', 'C' },
                                                                  { '*', '0', '#', 'D' } };

// Arduino pins for keypad column connections
const byte colPins[Hardware::KEYPAD_COLS] = { Hardware::KEYPAD_COL_PINS[0], Hardware::KEYPAD_COL_PINS[1], Hardware::KEYPAD_COL_PINS[2], Hardware::KEYPAD_COL_PINS[3] };
// Arduino pins for keypad row connections
const byte rowPins[Hardware::KEYPAD_ROWS] = { Hardware::KEYPAD_ROW_PINS[0], Hardware::KEYPAD_ROW_PINS[1], Hardware::KEYPAD_ROW_PINS[2], Hardware::KEYPAD_ROW_PINS[3] };

// Global keypad object
extern Keypad keypad;
// Flag indicating if we're in edit mode
extern bool editFlag;
// Language buffer loaded from PROGMEM
extern Language LANG_BUFFER;

/**
 * Display the splash screen animation on startup
 */
void splashScreen();

/**
 * Display language selection screen
 * @param idx Current language index
 * @return Selected language index
 */
uint8_t langConfigScreen(uint8_t idx);

/**
 * Get elapsed seconds since startup
 * Handles millis() overflow automatically
 * @return Total seconds elapsed
 */
inline uint64_t seconds() {
  static uint32_t previousMillis = 0;
  static uint64_t totalMillis = 0;
  uint32_t currentMillis = millis();
  uint32_t delta;
  // Handle millisecond counter overflow
  if (currentMillis >= previousMillis) {
    delta = currentMillis - previousMillis;
  } else {
    delta = (0xFFFFFFFFUL - previousMillis) + currentMillis + 1;
  }
  totalMillis += delta;
  previousMillis = currentMillis;
  return totalMillis / 1000;
}

/**
 * Display tank volume input screen
 * @param tankVolumeBuf Localized label for tank volume
 * @param editMode Whether to allow editing
 * @param tankVolume Current tank volume value
 * @return Entered tank volume, or -1 if cancelled
 */
uint32_t tankVolumeScreen(const char *tankVolumeBuf, bool editMode, uint32_t tankVolume);

/**
 * Display pump amount input screen
 * @param amountBuf Localized label for amount
 * @param pumpIndex Which pump (0-4)
 * @param editMode Whether to allow editing
 * @param amount Current amount value
 * @return Entered amount, or -1 if cancelled
 */
uint16_t pumpAmountScreen(const char *amountBuf, uint8_t pumpIndex, bool editMode, uint16_t amount);

/**
 * Display pump duration input screen
 * @param intervalBuf Localized label for duration
 * @param pumpIndex Which pump (0-4)
 * @param editMode Whether to allow editing
 * @param duration Current interval duration value in milliseconds
 * @return Entered duration, or -1 if cancelled
 */
uint16_t pumpIntervalScreen(const char *intervalBuf, uint8_t pumpIndex, bool editMode, uint16_t duration);

/**
 * Display main/idle screen
 * @param mainScreenBuf Localized main screen label
 * @param NoTaskBuf Localized no task message
 */
void mainScreen(const char *mainScreenBuf, const char *NoTaskBuf);

/**
 * Print string to LCD with custom glyphs support
 * Replaces certain characters with LCD custom characters
 * @param str String to print
 * @param length Number of characters to print
 */
void lcdPrintWithGlyphs(const char *str, uint8_t length);

/**
 * Display the current time on LCD
 * @param currentTime Time in seconds since midnight
 */
void showTime(uint64_t currentTime);

/**
 * Display time setup screen for setting current time
 * @return Time offset from millis(), or -1 if cancelled
 */
uint64_t timeSetupScreen(const char *label = "");

/**
 * Display water threshold configuration screen
 * @param thresholdBuf Localized label for threshold
 * @param editMode Whether to allow editing
 * @param lowThreshold Current low threshold value
 * @param highThreshold Current high threshold value
 * @return 1 if modified, 0 if not modified, -1 if cancelled
 */
uint8_t waterThresholdScreen(const char *thresholdBuf, bool editMode, int16_t lowThreshold, int16_t highThreshold);

/**
 * Handle editing pump amount (view + optional edit on followup key)
 * @param idx Pump index (0-4)
 */
void handleEditAmount(uint8_t idx);

/**
 * Handle editing tank volume (view + optional edit on followup key)
 */
void handleEditTankVolume(const char *tankTitle);

// /**
//  * Handle editing pump duration
//  * @param idx Pump index (0-4)
//  */
// void handleEditPumpDuration(uint8_t idx);

/**
 * Display a numeric input screen
 * @param label Title displayed on first line
 * @param format Format string with placeholder "_" for digits (PROGMEM)
 * @param entryCol Column where digit entry starts (LCD column 0-15)
 * @param maxDigits Maximum number of digits allowed
 * @param value Initial/current value
 * @param editMode If true, allows editing; if false, shows value only
 * @param unit Optional unit label (e.g., "ml", "l") displayed after digits
 * @return Entered value, or -1 if cancelled
 */
uint32_t editNumberScreen(const char *label, const char *format, uint8_t entryCol, uint8_t maxDigits, uint32_t value, bool editMode, const char *unit = nullptr);


/*
  * Handle editing water thresholds (view + optional edit on followup key)
  * User edits Inlet and Outlet thresholds sequentially.
  * If user enters invalid values, calls police.
*/
void handleThreshold();

void lightTimeScreen(uint64_t *lightofftime, uint64_t *lightontime);

#endif
