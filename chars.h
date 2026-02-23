/**
 * ============================================================================
 * CHARS.H - Custom LCD Character Definitions
 * ============================================================================
 * 
 * Defines custom 5x8 pixel glyphs for LCD display.
 * Supports language-specific characters not available in standard ASCII.
 * 
 * Supported Custom Glyphs by Language:
 *   - Polish: Special characters (ó, ż, etc.)
 *   - Russian: Cyrillic characters
 *   - French: Accented characters (é, è, ê, ù, etc.)
 *   - Spanish: Accented characters (á, ú, etc.)
 *   - Portuguese: Accented characters (ã, õ, etc.)
 * 
 * LCD has 8 custom character slots (0-7). This header manages loading
 * language-specific glyphs into these slots dynamically.
 */

#ifndef CUSTOM_CHARS_H
#define CUSTOM_CHARS_H

#include "display.h"
#include "debug.h"

#pragma once
#include <Arduino.h>
#include <avr/pgmspace.h>
#include <stdint.h>

// Number of available custom character slots on LCD display
#define LCD_AVAILABLE_CHAR_SLOTS 8

/**
 * Structure representing a set of custom glyphs for a language
 */
struct LanguageGlyphSet {
  const uint8_t (*glyphs)[8];  ///< Pointer to array of 8-byte glyph definitions
  uint8_t glyphCount;          ///< Number of glyphs in this set
};
// ============================================================================
// Drop animation frames (used in splash screen)
// ============================================================================
const uint8_t dropChars[4][8] PROGMEM = {
  { 4, 4, 14, 14, 14, 31, 31, 31 },
  { 1, 2, 2, 2, 2, 3, 1, 0 },
  { 31, 31, 31, 31, 31, 15, 3, 31 },
  { 16, 24, 24, 24, 24, 24, 16, 0 },
};

// ============================================================================
// Language-specific glyph sets
// ============================================================================

// Polish glyphs for special Polish characters
const uint8_t glyphs_polish[][8] PROGMEM = {
  { 0x00, 0x00, 0x0e, 0x11, 0x1f, 0x10, 0x0e, 0x01 },  // ę
  { 0x00, 0x04, 0x04, 0x06, 0x0C, 0x04, 0x04, 0x06 },
  { 0x02, 0x04, 0x00, 0x0E, 0x11, 0x11, 0x11, 0x0E }
};

// Russian/Cyrillic glyphs for Russian language support
const uint8_t glyphs_russian[][8] PROGMEM = {
  { 0x00, 0x00, 0x00, 0x11, 0x13, 0x15, 0x19, 0x11 },
  { 0x00, 0x0A, 0x04, 0x11, 0x13, 0x15, 0x19, 0x11 },
  { 0x1F, 0x11, 0x11, 0x0F, 0x03, 0x05, 0x09, 0x11 },
  { 0x00, 0x00, 0x00, 0x1E, 0x01, 0x0E, 0x01, 0x1E },
  { 0x00, 0x00, 0x00, 0x11, 0x11, 0x1D, 0x13, 0x1D },
  { 0x00, 0x00, 0x00, 0x18, 0x08, 0x0E, 0x09, 0x0E },
  { 0x00, 0x00, 0x00, 0x07, 0x09, 0x09, 0x1F, 0x11 },
  { 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00, 0x00, 0x00 }
};

// French glyphs for accented characters (é, è, ê, à, ù, etc.)
const uint8_t glyphs_francais[][8] PROGMEM = {
  { 0x00, 0x00, 0x00, 0x0E, 0x10, 0x10, 0x0E, 0x04 },
  { 0x02, 0x04, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E },
  { 0x02, 0x04, 0x1F, 0x10, 0x1F, 0x10, 0x10, 0x1F },
  { 0x04, 0x0A, 0x00, 0x0E, 0x01, 0x1F, 0x11, 0x0E },
  { 0x00, 0x14, 0x0A, 0x00, 0x1E, 0x11, 0x11, 0x11 }
};

// Spanish glyphs for accented characters
const uint8_t glyphs_espanol[][8] PROGMEM = {
  { 0x00, 0x14, 0x0A, 0x00, 0x1E, 0x11, 0x11, 0x11 }
};

// Portuguese glyphs for accented and special characters (ã, õ, ç, etc.)
const uint8_t glyphs_portugues[][8] PROGMEM = {
  { 0x04, 0x0A, 0x00, 0x0E, 0x11, 0x1E, 0x10, 0x0E },
  { 0x14, 0x0A, 0x00, 0x0E, 0x01, 0x1F, 0x11, 0x0E },
  { 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00 },
  { 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00 },
  { 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00 },
  { 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00 },
  { 0x00, 0x00, 0x0E, 0x11, 0x1F, 0x10, 0x0E, 0x00 }
};

// ============================================================================
// Glyph set mapping for all supported languages
// ============================================================================
// Index corresponds to LANGUAGES array indices in language.h
static const LanguageGlyphSet GLYPH_SETS[10] PROGMEM = {
  { glyphs_polish, 3 },     // 0: Polish
  { nullptr, 0 },           // 1: English (uses ASCII)
  { glyphs_russian, 8 },    // 2: Russian
  { nullptr, 0 },           // 3: German (uses ASCII)
  { glyphs_francais, 5 },   // 4: French
  { glyphs_espanol, 1 },    // 5: Spanish
  { nullptr, 0 },           // 6: Italian (uses ASCII)
  { glyphs_portugues, 7 },  // 7: Portuguese
  { nullptr, 0 },           // 8: Turkish (uses ASCII)
  { nullptr, 0 }            // 9: Czech (uses ASCII)
};

// Temporary buffer for building animated characters
uint8_t scratch[8];

/**
 * Create a frame for drop animation reveal effect
 * Constructs an intermediate animation frame for the water drop splash screen
 * @param charIndex Which drop character (0-3)
 * @param revealRows How many rows to reveal from bottom
 * @param scratchBuf Output buffer for the frame
 */
void makeRevealFrame(uint8_t charIndex, uint8_t revealRows,
                     uint8_t scratchBuf[]) {
  // Build frame by reading drop character and masking rows
  for (uint8_t row = 0; row < 8; row++) {
    uint8_t pix = pgm_read_byte(&dropChars[charIndex][row]);
    // Only show bottom 'revealRows' rows
    if (row < 8 - revealRows)
      pix = 0;
    scratchBuf[row] = pix;
  }
}

/**
 * Animate drop icon by creating intermediate frames
 * Used in splash screen animation
 * @param slots Array of 4 LCD character slot indices
 * @param revealRows How many rows to reveal
 * @param scratchBuf Work buffer for frame construction
 */
void animateIcon(uint8_t slots[4], uint8_t revealRows, uint8_t scratchBuf[]) {
  for (uint8_t i = 0; i < 4; i++) {
    makeRevealFrame(i, revealRows, scratchBuf);
    lcd.createChar(slots[i], scratchBuf);
  }
}

void loadCharSet(const uint8_t (*charset)[8], uint8_t count,
                 uint8_t startSlot = 0) {
  uint8_t buf[8];
  for (uint8_t i = 0; i < count; i++) {
    for (uint8_t b = 0; b < 8; b++) {
      buf[b] = pgm_read_byte(&charset[i][b]);
    }
    lcd.createChar(startSlot + i, buf);
  }
}

void loadGlyphSet(uint8_t langIndex) {
  LanguageGlyphSet set;
  memcpy_P(&set, &GLYPH_SETS[langIndex], sizeof(LanguageGlyphSet));
  if (!set.glyphs || set.glyphCount == 0)
    return;
  if (langIndex == 2) {
    SerialPrint(CHARS, F("DO NOT USE loadGlyphSet for Russian\nSince Russian has too many letters, CGRAM is insufficient."));
    Serial.println(F("Ignoring. Continuing..."));
  };
  loadCharSet(set.glyphs, set.glyphCount, 0);
}

#endif
