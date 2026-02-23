/**
 * ============================================================================
 * SCREENS_UTILS.CPP - Screen Utilities and Dynamic Glyph Cache
 * ============================================================================
 */

#include "screens.h"
#include "language.h"
#include "display.h"
#include "chars.h"
#include <Arduino.h>
#include <stddef.h>

Language LANG_BUFFER;

extern const char keys[Hardware::KEYPAD_ROWS][Hardware::KEYPAD_COLS];
extern const byte rowPins[Hardware::KEYPAD_ROWS];
extern const byte colPins[Hardware::KEYPAD_COLS];

Keypad keypad(makeKeymap(keys), const_cast<byte *>(rowPins), const_cast<byte *>(colPins), Hardware::KEYPAD_ROWS, Hardware::KEYPAD_COLS);
bool editFlag = false;

// Dynamic Glyph Cache: tracks which Master Library index is in physical slots 0-7
static uint8_t slotCache[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
static uint8_t nextSlot = 0;

void readLanguageField(uint8_t idx, uint8_t offset, char *dest, uint8_t len) {
  const void *base = static_cast<const void *>(&LANGUAGES[idx % LANG_COUNT]);
  memcpy_P(dest, reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(base) + offset), len);
  dest[len] = '\0';
}

Language readLanguage(uint8_t languageIndex) {
  Language result;
  memcpy_P(&result, &LANGUAGES[languageIndex % LANG_COUNT], sizeof(Language));
  return result;
}

void lcdPrintWithGlyphs(const char *str, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    uint8_t c = static_cast<uint8_t>(str[i]);
    if (c < 0x80) {
      lcd.write(c);
      continue;
    }
    uint8_t libIdx = c - 128; // octal \200 is 128
    if (libIdx >= Glyphs::LIBRARY_SIZE) {
      lcd.write('?'); // Safety: fallback character if glyph missing
      continue;
    }
    int8_t slot = -1;
    for (uint8_t s = 0; s < 8; s++) {
      if (slotCache[s] == libIdx) { slot = s; break; }
    }
    if (slot == -1) {
      slot = nextSlot;
      slotCache[nextSlot] = libIdx;
      uint8_t bitmask[8];
      for (uint8_t b = 0; b < 8; b++) {
        bitmask[b] = pgm_read_byte(&Glyphs::MASTER_LIBRARY[libIdx][b]);
      }
      lcd.createChar(nextSlot, bitmask);
      nextSlot = (nextSlot + 1) % 8;
    }
    lcd.write(static_cast<uint8_t>(slot));
  }
}
