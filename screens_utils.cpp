/**
 * ============================================================================
 * SCREENS_UTILS.CPP - UTF-8 Decoder and Dynamic Glyph Cache
 * ============================================================================
 */

#include "screens.h"
#include "language.h"
#include "display.h"
#include "chars.h"
#include <Arduino.h>

Language LANG_BUFFER;
extern const char keys[Hardware::KEYPAD_ROWS][Hardware::KEYPAD_COLS];
extern const byte rowPins[Hardware::KEYPAD_ROWS];
extern const byte colPins[Hardware::KEYPAD_COLS];
Keypad keypad(makeKeymap(keys), const_cast<byte *>(rowPins), const_cast<byte *>(colPins), Hardware::KEYPAD_ROWS, Hardware::KEYPAD_COLS);
bool editFlag = false;

static uint16_t slotCache[8] = { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF };
static uint8_t nextSlot = 0;

void readLanguageField(uint8_t idx, uint8_t offset, char *dest, uint8_t len) {
  memcpy_P(dest, reinterpret_cast<const void *>(reinterpret_cast<uintptr_t>(static_cast<const void *>(&LANGUAGES[idx % LANG_COUNT])) + offset), len);
}

Language readLanguage(uint8_t languageIndex, Language *dest) {
  memcpy_P(dest, &LANGUAGES[languageIndex % LANG_COUNT], sizeof(Language));
  return *dest;
}

static int8_t getPhysicalSlot(uint16_t unicode) {
  for (uint8_t s = 0; s < 8; s++) {
    if (slotCache[s] == unicode) return s;
  }
  for (uint8_t i = 0; i < Glyphs::LIBRARY_SIZE; i++) {
    if (pgm_read_word(&Glyphs::MASTER_LIBRARY[i].id) == unicode) {
      uint8_t bitmask[8];
      memcpy_P(bitmask, Glyphs::MASTER_LIBRARY[i].data, 8);
      lcd.createChar(nextSlot, bitmask);
      slotCache[nextSlot] = unicode;
      uint8_t assigned = nextSlot;
      nextSlot = (nextSlot + 1) % 8;
      return assigned;
    }
  }
  return -1;
}

void lcdPrintWithGlyphs(const char *str, uint8_t length) {
  const uint8_t *p = reinterpret_cast<const uint8_t *>(str);
  uint8_t printed = 0;
  while (*p && printed < length) {
    uint16_t unicode = 0;
    uint8_t b = *p++;
    if (b < 0x80) unicode = b;
    else if ((b & 0xE0) == 0xC0) {
      unicode = ((b & 0x1F) << 6) | (*p++ & 0x3F);
    } else if ((b & 0xF0) == 0xE0) {
      uint8_t b2 = *p++;
      uint8_t b3 = *p++;
      unicode = ((b & 0x0F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
    } else continue;

    if (unicode < 128) lcd.write(static_cast<uint8_t>(unicode));
    else {
      int8_t slot = getPhysicalSlot(unicode);
      lcd.write(slot >= 0 ? static_cast<uint8_t>(slot) : ' ');
    }
    printed++;
  }
}
