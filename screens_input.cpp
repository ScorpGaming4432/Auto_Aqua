/**
 * ============================================================================
 * SCREENS_INPUT.CPP - Numeric Input Screen Implementation
 * ============================================================================
 */

#include "screens.h"
#include "display.h"
#include "appstate.h"
#include "storage.h"
#include "debug.hpp"
#include <Arduino.h>

/**
 * Generic numeric input screen with real-time editing
 * Handles cursor blinking, multi-digit number entry, and validation
 */
uint32_t editNumberScreen(const char *label, const char *format, uint8_t entryCol, uint8_t maxDigits, uint32_t value, bool editMode, const char *unit) {
  SerialPrint(KEYPAD_INPUT, "Opening numeric editor label=", label, " maxDigits=", maxDigits, " initialValue=", value);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcdPrintWithGlyphs(label, 16);
  lcd.setCursor(0, 1);

  // Copy format into a local RAM buffer so we can inspect characters
  char fmtBuf[17];
  strncpy(fmtBuf, format, 16);
  fmtBuf[16] = '\0';
  lcdPrintWithGlyphs(fmtBuf, 16);

  // Determine whether the format already contains a unit character
  bool formatHasUnit = false;
  uint8_t unitPos = entryCol + maxDigits;
  if (unit && unitPos < sizeof(fmtBuf) - 1 && fmtBuf[unitPos] != '\0' && fmtBuf[unitPos] != ' ' && fmtBuf[unitPos] != '_') {
    formatHasUnit = true;
  }

  // Animation timing for cursor blink (500ms period)
  unsigned long lastBlink = millis();
  bool showCursor = false;
  bool digitsEntered = (value != UNSET_U32);
  uint8_t lastDigitPos = entryCol + maxDigits - 1;
  char lastDigitChar = ' ';
  uint8_t curLen = 0;

  /**
   * Lambda function to redraw the numeric input field
   */
  auto redrawNumber = [&](uint32_t val) {
    lcd.setCursor(entryCol, 1);
    for (uint8_t i = 0; i < maxDigits; i++)
      lcd.print('_');

    if (!digitsEntered) {
      curLen = 0;
      lastDigitPos = entryCol + maxDigits - 1;
      lastDigitChar = ' ';
      if (unit && !formatHasUnit) {
        lcd.setCursor(unitPos, 1);
        lcd.print(unit);
      }
      return;
    }

    char tmp[12]; // Increased size for safety (uint32 can be 10 digits)
    uint8_t idx = 0;
    unsigned long v = static_cast<unsigned long>(val);

    if (v == 0) {
      tmp[idx++] = '0';
    } else {
      char rev[12];
      uint8_t ri = 0;
      while (v > 0 && ri < sizeof(rev)) {
        rev[ri++] = '0' + (v % 10);
        v /= 10;
      }
      for (int8_t i = ri - 1; i >= 0; --i)
        tmp[idx++] = rev[i];
    }
    tmp[idx] = '\0';

    if (idx > maxDigits) {
      const char *p = tmp + (idx - maxDigits);
      lcd.setCursor(entryCol, 1);
      lcd.print(p);
      curLen = maxDigits;
      lastDigitPos = entryCol + maxDigits - 1;
      lastDigitChar = p[maxDigits - 1];
    } else {
      uint8_t start = entryCol + (maxDigits - idx);
      lcd.setCursor(start, 1);
      lcd.print(tmp);
      curLen = idx;
      if (curLen == 0) {
        lastDigitPos = entryCol + maxDigits - 1;
        lastDigitChar = ' ';
      } else {
        lastDigitPos = start + curLen - 1;
        lastDigitChar = tmp[curLen - 1];
      }
    }

    if (unit && !formatHasUnit) {
      lcd.setCursor(unitPos, 1);
      lcd.print(unit);
    }
  };

  redrawNumber(value);
  bool localEdit = editMode;
  uint32_t number = value;

  while (true) {
    char key = keypad.getKey();

    if (localEdit && millis() - lastBlink >= 500) {
      lastBlink = millis();
      showCursor = !showCursor;
      lcd.setCursor(lastDigitPos, 1);
      if (showCursor)
        lcd.print('|');
      else
        lcd.print(lastDigitChar);
    }

    if (!key) {
      delay(10);
      continue;
    }

    if (!localEdit) {
      if (key == '#') {
        SerialPrint(KEYPAD_INPUT, "Numeric editor entering edit mode");
        localEdit = true;
        if (number != UNSET_U32)
          digitsEntered = true;
        else {
          digitsEntered = false;
          number = 0;
        }
        redrawNumber(number);
      } else if (key == '*') {
        SerialPrint(KEYPAD_INPUT, "Numeric editor cancelled before editing");
        return UNSET_U32;
      }
      continue;
    }

    if (key == '*') {
      SerialPrint(KEYPAD_INPUT, "Numeric editor clear/cancel key pressed");
      if (!digitsEntered)
        return UNSET_U32;
      number = 0;
      digitsEntered = false;
      redrawNumber(number);
      continue;
    }

    if (key == '#') {
      SerialPrint(KEYPAD_INPUT, "Numeric editor confirm key pressed");
      if (!digitsEntered)
        return UNSET_U32;
      SerialPrint(KEYPAD_INPUT, "Numeric editor returning value=", number);
      return number;
    }

    if (key >= '0' && key <= '9') {
      SerialPrint(KEYPAD_INPUT, "Numeric digit entered: ", key);
      if (number == UNSET_U32) number = 0;
      if (curLen < maxDigits) {
        digitsEntered = true;
        number = number * 10 + (key - '0');
        redrawNumber(number);
      } else {
        SerialPrint(KEYPAD_INPUT, "Numeric editor overflow: too many digits; returning UNSET");
        return UNSET_U32;
      }
      continue;
    }
  }
}
