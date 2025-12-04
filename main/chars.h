// TODO: REWRITE THIS WHOLE FILE TO SUPPORT MULTIPLE LANGUAGES AND GLYPHS (maybe long list of languages with their glyphs so that indexing is easier?)
// work with language.h/cpp

#ifndef CUSTOM_CHARS_H
#define CUSTOM_CHARS_H

#pragma once
#include <stdint.h>

// The count of custom character slots available in LCD CGRAM
#define LCD_CHAR_SLOTS 8

// For now, you only have the "drop" characters.
// Later you can add accents per-language.

extern const uint8_t dropChars[4][8];

// Generic function to load any char set into chosen slots.
void loadCharSet(const uint8_t (*charset)[8], uint8_t count, uint8_t startSlot = 0);

// Glyph manager API
// Register a glyph bitmap for a UTF-8 sequence (e.g. "ą" encoded in UTF-8)
// The bitmap should be 8 bytes, one per display row.
bool registerGlyph(const char* utf8, const uint8_t bitmap[8]);

// Ensure registered glyph bitmaps are loaded into CGRAM (calls createChar)
void loadRegisteredGlyphs(uint8_t startSlot = 0);

// Print a string using registered glyphs when available. This will map
// registered UTF-8 sequences to the allocated CGRAM slot and write the
// corresponding custom char. Falls back to printing ASCII characters or
// '?' for unsupported sequences.
void lcdPrintWithGlyphs(const char* s);

// Clear all registered glyphs (does not modify CGRAM)
void clearRegisteredGlyphs();

// Load a language-specific set of up to 8 custom glyphs into CGRAM slots 0..7.
// The glyphs are 8-byte arrays (one byte per LCD row). Placeholders are
// provided in `chars.cpp` and can be edited later to draw the actual bitmaps.
void loadLanguageGlyphs(uint8_t langIndex);
#endif