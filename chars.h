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
// Special glyphs
// ============================================================================

const uint8_t special_glyphs[][8] PROGMEM = {
  { 0b00000, 0b00000, 0b01110, 0b10001, 0b11111, 0b10000, 0b01110, 0b00001 },  // Polish: ę
  { 0b00010, 0b00100, 0b01110, 0b10000, 0b01110, 0b00001, 0b11110, 0b00000 },  // Polish: ś
  { 0b00010, 0b00100, 0b01110, 0b10000, 0b10000, 0b10001, 0b01110, 0b00000 },  // Polish: ć
  { 0b01100, 0b00100, 0b00100, 0b00110, 0b01100, 0b00100, 0b01110, 0b00000 },  // Polish: ł
  { 0B00010, 0B00100, 0B01110, 0B10001, 0B10001, 0B10001, 0B01110, 0B00000 },  // Polish: ó
  { 0B00000, 0B00000, 0B01110, 0B00001, 0B01111, 0B10001, 0B01111, 0B00011 },  // Polish: ą
  { 0B00010, 0B00100, 0B11111, 0B00010, 0B00100, 0B01000, 0B11111, 0B00000 },  // Polish: ź
  { 0b00000, 0b00000, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b00000 },  // Russian: и
  { 0b01010, 0b00100, 0b10001, 0b10011, 0b10101, 0b11001, 0b10001, 0b00000 },  // Russian: й
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
void makeRevealFrame(uint8_t charIndex, uint8_t revealRows, uint8_t scratchBuf[]) {
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

// TODO: Implement GLYPH_SETS array or remove this function
// void loadGlyphSet(uint8_t langIndex) {
//   LanguageGlyphSet set;
//   memcpy_P(&set, &GLYPH_SETS[langIndex], sizeof(LanguageGlyphSet));
//   if (!set.glyphs || set.glyphCount == 0)
//     return;
//   if (langIndex == 2) {
//     SerialPrint(CHARS, F("DO NOT USE loadGlyphSet for Russian\nSince Russian has too many letters, CGRAM is insufficient."));
//     Serial.println(F("Ignoring. Continuing..."));
//   };
//   loadCharSet(set.glyphs, set.glyphCount, 0);
// }

#endif
