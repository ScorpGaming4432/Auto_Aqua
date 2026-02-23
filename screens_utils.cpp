/**
 * ============================================================================
 * SCREENS_UTILS.CPP - Screen Utilities and Global State
 * ============================================================================
 */

#include "screens.h"
#include "language.h"
#include "display.h"
#include <Arduino.h>
#include <stddef.h>

// Language buffer loaded from PROGMEM into RAM
Language LANG_BUFFER;

// External declarations for keypad arrays defined in screens.h
extern const char keys[Hardware::KEYPAD_ROWS][Hardware::KEYPAD_COLS];
extern const byte rowPins[Hardware::KEYPAD_ROWS];
extern const byte colPins[Hardware::KEYPAD_COLS];

// Global keypad object - shared across all screen functions
Keypad keypad(makeKeymap(keys), const_cast<byte *>(rowPins), const_cast<byte *>(colPins), Hardware::KEYPAD_ROWS, Hardware::KEYPAD_COLS);

// Global edit mode flag
bool editFlag = false;

/**
 * Read a single field from a language structure in PROGMEM
 * @param idx Language index (0-9)
 * @param offset Byte offset within the Language structure
 * @param dest Destination RAM buffer
 * @param len Number of bytes to copy
 */
void readLanguageField(uint8_t idx, uint8_t offset, char *dest, uint8_t len) {
  const void *base = static_cast<const void *>(&LANGUAGES[idx % LANG_COUNT]);
  memcpy_P(dest, reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(base) + offset), len);
  dest[len] = '\0';
}

/**
 * Read a complete Language structure from PROGMEM
 * @param idx Language index (0-9)
 * @return Language structure for the selected language
 */
Language readLanguage(uint8_t languageIndex) {
  Language result;
  memcpy_P(&result, &LANGUAGES[languageIndex % LANG_COUNT], sizeof(Language));
  return result;
}

void lcdPrintWithGlyphs(const char *str, uint8_t length) {
  for (uint8_t i = 0; i < length; ++i) {
    lcd.write(static_cast<uint8_t>(str[i]));
  }
}
