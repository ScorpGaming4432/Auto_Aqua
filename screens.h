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
 * Key Mappings:
 *   1-5: Edit pump amounts
 *   0:   Show current time
 *   D:   Edit tank volume
 *   A/B: Navigate menus
 *   #:   Confirm selection
 *   *:   Cancel/Back
 *   C:   Water level measurement
 *   P:   Edit pump duration
 */

#ifndef SCREENS_H
#define SCREENS_H

#include <Keypad.h>
#include <stdint.h>

// Keypad configuration - 4x4 matrix
const byte ROWS = 4;
const byte COLS = 4;

// Keypad layout definition
const char keys[ROWS][COLS] = { { '1', '2', '3', 'A' },
                                { '4', '5', '6', 'B' },
                                { '7', '8', '9', 'C' },
                                { '*', '0', '#', 'D' } };

// Arduino pins for keypad column connections
const byte colPins[COLS] = { 31, 33, 35, 37 };
// Arduino pins for keypad row connections
const byte rowPins[ROWS] = { 30, 32, 34, 36 };

// Global keypad object
extern Keypad keypad;
// Flag indicating if we're in edit mode
extern bool editFlag;

/**
 * Display the splash screen animation on startup
 */
void splashScreen();

/**
 * Display language selection screen
 * @param idx Current language index
 * @param editMode Whether in edit mode or view mode
 * @return Selected language index
 */
uint8_t langConfigScreen(uint8_t idx, bool editMode);

/**
 * Display tank volume input screen
 * @param tankVolumeBuf Localized label for tank volume
 * @param editMode Whether to allow editing
 * @param tankVolume Current tank volume value
 * @return Entered tank volume, or -1 if cancelled
 */
int32_t tankVolumeScreen(const char *tankVolumeBuf, bool editMode,
                         uint32_t tankVolume);

/**
 * Display pump amount input screen
 * @param amountBuf Localized label for amount
 * @param pumpIndex Which pump (0-4)
 * @param editMode Whether to allow editing
 * @param amount Current amount value
 * @return Entered amount, or -1 if cancelled
 */
int16_t pumpAmountScreen(const char *amountBuf, uint8_t pumpIndex, bool editMode,
                         uint32_t amount);

/**
 * Display pump duration input screen
 * @param durationBuf Localized label for duration
 * @param pumpIndex Which pump (0-4)
 * @param editMode Whether to allow editing
 * @param duration Current duration value in milliseconds
 * @return Entered duration, or -1 if cancelled
 */
uint64_t pumpDurationScreen(const char *durationBuf, uint8_t pumpIndex, bool editMode, uint64_t duration);

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
uint64_t timeSetupScreen();

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
 * Display water threshold configuration screen
 * @param thresholdBuf Localized label for threshold
 * @param editMode Whether to allow editing
 * @param lowThreshold Current low threshold value
 * @param highThreshold Current high threshold value
 * @return 1 if modified, 0 if not modified, -1 if cancelled
 */
int8_t waterThresholdScreen(const char *thresholdBuf, bool editMode,
                             int16_t lowThreshold, int16_t highThreshold);

#endif
