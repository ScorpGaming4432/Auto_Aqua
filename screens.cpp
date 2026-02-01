/**
 * ============================================================================
 * SCREENS.CPP - Screen Display and Input Handling Implementation
 * ============================================================================
 * 
 * Implements all LCD display screens for the aquarium system, including:
 *   - Splash screen animation
 *   - Language selection interface
 *   - Tank volume configuration
 *   - Pump amount configuration
 *   - Pump duration configuration
 *   - Time setup interface
 *   - Main menu display
 *   - Numeric input editing with cursor feedback
 * 
 * The numeric input system supports:
 *   - Real-time cursor blinking
 *   - Multi-digit number entry (0-9999999)
 *   - Backspace/cancel functionality
 *   - Confirmation with '#' key
 * 
 */

#include "screens.h"
#include "chars.h"
#include "display.h"
#include "language.h"
#include "water.h"
#include <Arduino.h>
#include <Keypad.h>

// ============================================================================
// Language data reading utilities
// ============================================================================

/**
 * Read a single field from a language structure in PROGMEM
 * More memory-efficient than reading the entire Language structure
 * Uses memcpy_P to copy from Flash (PROGMEM) to RAM buffer
 * @param idx Language index (0-9)
 * @param offset Byte offset within the Language structure
 * @param dest Destination RAM buffer
 * @param len Number of bytes to copy
 */
void readLanguageField(uint8_t idx, uint8_t offset, char *dest, uint8_t len) {
  const void *base = (const void *)&LANGUAGES[idx % LANG_COUNT];
  memcpy_P(dest, (const void *)((uintptr_t)base + offset), len);
  dest[len] = '\0';
}

/**
 * Read a complete Language structure from PROGMEM
 * Use sparingly as this loads the entire structure into RAM
 * @param idx Language index (0-9)
 * @return Language structure for the selected language
 */
Language readLanguage(uint8_t idx) {
  Language result;
  memcpy_P(&result, &LANGUAGES[idx % LANG_COUNT], sizeof(Language));
  return result;
}

// ============================================================================
// Keypad and global state
// ============================================================================

// Global keypad object - shared across all screen functions
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
// Global edit mode flag
bool editFlag = false;

// ============================================================================
// Splash screen - displayed at startup
// ============================================================================

/**
 * Display animated splash screen
 * Shows "AUTO AQUA" with animated water drop effect
 */
void splashScreen() {
  // Initialize LCD display and turn on backlight
  lcd.init();
  lcd.backlight();
  lcd.clear();

  // Display title
  lcd.setCursor(3, 0);
  lcd.print("AUTO");
  lcd.setCursor(4, 1);
  lcd.print("AQUA");

  // Define character slots for animation
  uint8_t slots[4] = { 0, 1, 2, 3 };

  // Animate water drop reveal effect (8 frames)
  for (uint8_t r = 0; r <= 8; r++) {
    // Update drop animation in top-right corner
    lcd.setCursor(11, 0);
    lcd.write(0);
    lcd.setCursor(10, 1);
    lcd.write(1);
    lcd.write(2);
    lcd.write(3);

    // Draw animation frame
    animateIcon(slots, r, scratch);
    delay(80);  // Frame delay for smooth animation
  }

  delay(1000);  // Display completed splash for 1 second
}

// ============================================================================
// Language selection screen
// ============================================================================

/**
 * Display language selection screen
 * Allows user to cycle through 10 languages and select one
 * Keys: A/B to navigate, # to confirm, * to cancel, 0-9 for direct selection
 * @param idx Current language index
 * @param editMode If true, allows editing (required for initial setup)
 * @return Selected language index
 */
uint8_t langConfigScreen(uint8_t idx, bool editMode) {
  lcd.clear();
  // Load custom glyphs for the selected language
  loadGlyphSet(idx);

  // Read language name and prompt from PROGMEM
  char langName[LANG_NAME_LEN + 1];
  char langPrompt[LANG_PROMPT_LEN + 1];
  readLanguageField(idx, offsetof(Language, langName), langName,
                    LANG_NAME_LEN);
  readLanguageField(idx, offsetof(Language, langPrompt), langPrompt,
                    LANG_PROMPT_LEN);

  // Display language name and navigation instructions
  lcd.setCursor(0, 0);
  lcdPrintWithGlyphs(langName, LANG_NAME_LEN);
  lcd.setCursor(0, 1);
  lcd.print("Num=");
  lcdPrintWithGlyphs(langPrompt, LANG_PROMPT_LEN);
  lcd.print("  #->");

  uint8_t newlang = idx;
  while (true) {
    char key = keypad.getKey();

    if (key) {
      // Numeric key: select language directly
      if (key >= '0' && key <= '9') {
        newlang = key - '0';
      }
      // Key A: next language
      else if (key == 'A') {
        newlang = (newlang + 1) % LANG_COUNT;
      }
      // Key B: previous language
      else if (key == 'B') {
        newlang = (newlang + LANG_COUNT - 1) % LANG_COUNT;
      }
      // Key #: confirm selection
      else if (key == '#') {
        return newlang;
      }
      // Key *: cancel
      else if (key == '*') {
        return idx;
      }

      // Update display if language changed
      if (newlang != idx) {
        idx = newlang;
        loadGlyphSet(idx);
        readLanguageField(idx, offsetof(Language, langName), langName,
                          LANG_NAME_LEN);
        readLanguageField(idx, offsetof(Language, langPrompt), langPrompt,
                          LANG_PROMPT_LEN);
        lcd.setCursor(0, 0);
        lcdPrintWithGlyphs(langName, LANG_NAME_LEN);
        lcd.setCursor(4, 1);
        lcdPrintWithGlyphs(langPrompt, LANG_PROMPT_LEN);
      }
    }

    delay(100);
  }
}

// Formatki LCD do PROGMEM


// ============================================================================
// Numeric input screen - shared by tank volume, pump amount, and duration
// ============================================================================

/**
 * Generic numeric input screen with real-time editing
 * Handles cursor blinking, multi-digit number entry, and validation
 * 
 * Keys:
 *   0-9:  Enter digit
 *   #:    Confirm entry (must have entered at least one digit)
 *   *:    Clear/backspace or cancel
 *   A/B:  Move cursor (for future enhancement)
 * 
 * @param label Title displayed on first line
 * @param format Format string with placeholder \"_\" for digits (PROGMEM)
 * @param entryCol Column where digit entry starts (LCD column 0-15)
 * @param maxDigits Maximum number of digits allowed
 * @param value Initial/current value
 * @param editMode If true, allows editing; if false, shows value only
 * @param unit Optional unit label (e.g., \"ml\", \"l\") displayed after digits
 * @return Entered value, or -1 if cancelled
 */
int32_t editNumberScreen(const char *label, const char *format,
                         uint8_t entryCol, uint8_t maxDigits, uint32_t value,
                         bool editMode, const char *unit = nullptr) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(label);
  lcd.setCursor(0, 1);
  // Copy format string from PROGMEM to RAM buffer
  // char fmtBuf[20];
  // strncpy_P(fmtBuf, format, sizeof(fmtBuf) - 1);
  // fmtBuf[sizeof(fmtBuf) - 1] = '\0';
  lcd.print(format);

  // Animation timing for cursor blink (500ms period)
  unsigned long lastBlink = millis();
  bool showCursor = false;
  // Flag to track if user has entered any digits
  bool digitsEntered = (value != 0);
  // Position of the last digit entered (for cursor placement)
  uint8_t lastDigitPos = entryCol + maxDigits - 1;
  // Character at last digit position (for cursor restoration)
  char lastDigitChar = ' ';
  // Current length of entered number
  uint8_t curLen = 0;

  /**
   * Lambda function to redraw the numeric input field
   * Updates display with current value, handling alignment and formatting
   */
  auto redrawNumber = [&](uint32_t val) {
    // Clear digit field with underscores
    lcd.setCursor(entryCol, 1);
    for (uint8_t i = 0; i < maxDigits; i++)
      lcd.print('_');

    // If no digits entered, reset tracking variables
    if (!digitsEntered) {
      curLen = 0;
      lastDigitPos = entryCol + maxDigits - 1;
      lastDigitChar = ' ';
      return;
    }

    // Convert number to string for display
    char tmp[8];  // max 7 digits + null terminator
    uint8_t idx = 0;
    unsigned long v = (unsigned long)val;

    // Handle zero special case
    if (v == 0) {
      tmp[idx++] = '0';
    } else {
      // Convert to digits (reversed)
      char rev[8];
      uint8_t ri = 0;
      while (v > 0 && ri < sizeof(rev)) {
        rev[ri++] = '0' + (v % 10);
        v /= 10;
      }
      // Reverse digits to get correct order
      for (int8_t i = ri - 1; i >= 0; --i)
        tmp[idx++] = rev[i];
    }
    tmp[idx] = '\0';

    // If number exceeds max digits, display rightmost digits
    if (idx > maxDigits) {
      const char *p = tmp + (idx - maxDigits);
      lcd.setCursor(entryCol, 1);
      lcd.print(p);
      curLen = maxDigits;
      lastDigitPos = entryCol + maxDigits - 1;
      lastDigitChar = p[maxDigits - 1];
    } else {
      // Right-align number in field
      uint8_t start = entryCol + (maxDigits - idx);
      lcd.setCursor(start, 1);
      lcd.print(tmp);
      curLen = idx;
      // Update cursor tracking
      if (curLen == 0) {
        lastDigitPos = entryCol + maxDigits - 1;
        lastDigitChar = ' ';
      } else {
        lastDigitPos = start + curLen - 1;
        lastDigitChar = tmp[curLen - 1];
      }
    }

    //Display unit label if provided
    if (unit) {
      lcd.setCursor(entryCol + maxDigits, 1);
      lcd.print(unit);
    }
  };

  // Initial display
  redrawNumber(value);
  bool localEdit = editMode;
  uint32_t number = value;

  // Main input loop
  while (true) {
    char key = keypad.getKey();

    // Handle cursor blink animation (500ms period)
    if (localEdit && millis() - lastBlink >= 500) {
      lastBlink = millis();
      showCursor = !showCursor;
      // Display cursor or digit at current position
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

    // In view mode: # enters edit mode, * cancels
    if (!localEdit) {
      if (key == '#') {
        // Enter edit mode
        localEdit = true;
        // Mark digits as entered if value is non-zero
        if (number > 0 && number != (uint32_t)-1)
          digitsEntered = true;
        else {
          digitsEntered = false;
          number = 0;
        }
        redrawNumber(number);
      } else if (key == '*') {
        return -1;
      }
      continue;
    }

    // In edit mode
    if (key == '*') {
      // Backspace: clear number if empty, or reset to 0
      if (!digitsEntered || number == 0)
        return -1;
      number = 0;
      digitsEntered = false;
      redrawNumber(number);
      continue;
    }

    if (key == '#') {
      // Confirm: must have entered digits
      if (!digitsEntered || number == 0)
        return -1;
      return (int32_t)number;
    }

    // Digit entry: 0-9
    if (key >= '0' && key <= '9') {
      // Check if we have room for more digits
      if (curLen < maxDigits) {
        digitsEntered = true;
        // Shift number left and add new digit
        number = number * 10 + (key - '0');
        redrawNumber(number);
      } else {
        // Too many digits
        number = (uint32_t)-1;
        return -1;
      }
      continue;
    }
  }
}

int32_t tankVolumeScreen(const char *tankVolumeBuf, bool editMode,
                         uint32_t tankVolume) {
  Serial.print("[TANK] entry: editMode=");
  Serial.print(editMode);
  Serial.print(" tankVolume=");
  Serial.println(tankVolume);
  if (tankVolume == (uint32_t)-1ULL) {
    Serial.println("[TANK] sentinel passed -> start edit mode at 0");
    tankVolume = 0;
    editMode = true;
  }
  return editNumberScreen(tankVolumeBuf, "<-* _______l #->", 4, 7, tankVolume,
                          editMode, "l");
}

int16_t pumpAmountScreen(const char *amountBuf, uint8_t pumpIndex, bool editMode,
                         uint32_t amount) {
  Serial.print("[AM] entry: editMode=");
  Serial.print(editMode);
  Serial.print(" amount=");
  Serial.print(amount);
  Serial.print(" index=");
  Serial.println(pumpIndex);
  // Make buffer large enough to include terminating NUL and copy full string (so '#' isn't lost)
  char _amBuf[LANG_AMOUNTTITLE_LEN + 1];
  strncpy(_amBuf, amountBuf, sizeof(_amBuf) - 1);
  _amBuf[sizeof(_amBuf) - 1] = '\0';
  for (int i = 0; _amBuf[i] != '\0'; i++) {
    if (_amBuf[i] == '#') {
      _amBuf[i] = '1' + pumpIndex;
      break;
    }
  }
  if (amount == (uint32_t)-1ULL) {
    Serial.println("[AM] sentinel passed -> start edit mode at 0");
    amount = 0;
    editMode = true;
  }
  return editNumberScreen(_amBuf, "<-* ______ml #->", 6, 6, amount, editMode);
}

uint64_t pumpDurationScreen(const char *durationBuf, uint8_t pumpIndex, bool editMode, uint64_t duration) {
  Serial.print("[DUR] entry: pumpIndex=");
  Serial.print(pumpIndex);
  Serial.print(" editMode=");
  Serial.print(editMode);
  Serial.print(" duration=");
  Serial.println((unsigned long)duration);

  // Prepare the buffer for the title, replacing '#' with the pump index
  // Use duration title length and ensure NUL is available
  char _durationBuf[LANG_DURATIONTITLE_LEN + 1];
  strncpy(_durationBuf, durationBuf, sizeof(_durationBuf) - 1);
  _durationBuf[sizeof(_durationBuf) - 1] = '\0';
  for (int i = 0; _durationBuf[i] != '\0'; i++) {
    if (_durationBuf[i] == '#') {
      _durationBuf[i] = '1' + pumpIndex;
      break;
    }
  }

  // Handle sentinel value for duration
  if (duration == (uint64_t)-1ULL) {
    Serial.println("[DUR] sentinel passed -> start edit mode at 0");
    duration = 0;
    editMode = true;
  }

  // Convert duration to 32-bit if it exceeds the limit
  uint32_t dur32 = (duration > 0xFFFFFFFFULL) ? 0xFFFFFFFFUL : (uint32_t)duration;

  // Call the editNumberScreen function to handle the editing
  int32_t res = editNumberScreen(_durationBuf, "<-* ______ms #->", 4, 7, dur32, editMode, "ms");

  // Handle the result
  if (res == -1) {
    return (uint64_t)-1;
  }

  return (uint64_t)(uint32_t)res;
}

void lcdPrintWithGlyphs(const char *str, uint8_t length) {
  for (uint8_t i = 0; i < length; ++i) {
    lcd.write((uint8_t)str[i]);
  }
}

uint64_t timeSetupScreen() {
  uint64_t nowSecs = seconds();
  uint32_t tod = (uint32_t)(nowSecs % 86400ULL);
  uint8_t hh = tod / 3600;
  uint8_t mm = (tod % 3600) / 60;
  uint8_t ss = tod % 60;

  auto showTime = [&](const char digits[6]) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(digits[0]);
    lcd.print(digits[1]);
    lcd.print(':');
    lcd.print(digits[2]);
    lcd.print(digits[3]);
    lcd.print(':');
    lcd.print(digits[4]);
    lcd.print(digits[5]);
    lcd.setCursor(0, 1);
    lcd.print("#=OK  *=Cancel");
  };

  char digits[7];
  digits[0] = '0' + (hh / 10);
  digits[1] = '0' + (hh % 10);
  digits[2] = '0' + (mm / 10);
  digits[3] = '0' + (mm % 10);
  digits[4] = '0' + (ss / 10);
  digits[5] = '0' + (ss % 10);
  digits[6] = '\0';

  showTime(digits);

  uint8_t pos = 0;
  unsigned long lastBlink = millis();
  bool showCursor = true;
  showTime(digits);

  while (true) {
    for (uint8_t i = 0; i < 6; ++i) {
      uint8_t col = (i < 2) ? i : ((i < 4) ? (i + 1) : (i + 2));
      lcd.setCursor(col, 0);
      if (i == pos && showCursor) {
        lcd.print('|');
      } else {
        lcd.print(digits[i]);
      }
    }

    if (millis() - lastBlink >= 500) {
      lastBlink = millis();
      showCursor = !showCursor;
    }

    char key = keypad.getKey();
    if (!key) {
      delay(30);
      continue;
    }

    if (key >= '0' && key <= '9') {
      digits[pos] = key;
      pos = (pos + 1) % 6;
      showCursor = true;
      continue;
    }

    if (key == 'A') {
      pos = (pos + 1) % 6;
      continue;
    }
    if (key == 'B') {
      pos = (pos + 6 - 1) % 6;
      continue;
    }
    if (key == '*') {
      Serial.println("[TIME] edit cancelled");
      return (uint32_t)-1;
    }
    if (key == '#') {
      uint8_t nh = (digits[0] - '0') * 10 + (digits[1] - '0');
      uint8_t nm = (digits[2] - '0') * 10 + (digits[3] - '0');
      uint8_t ns = (digits[4] - '0') * 10 + (digits[5] - '0');
      if (nh > 23)
        nh = nh % 24;
      if (nm > 59)
        nm = nm % 60;
      if (ns > 59)
        ns = ns % 60;

      uint32_t enteredSeconds =
        (uint32_t)nh * 3600UL + (uint32_t)nm * 60UL + (uint32_t)ns;
      uint64_t tmptimeoffset = seconds() - (uint64_t)enteredSeconds;
      Serial.print("[TIME] entered HH:MM:SS = ");
      Serial.print(nh);
      Serial.print(":");
      Serial.print(nm);
      Serial.print(":");
      Serial.println(ns);
      Serial.print("[TIME] tmptimeoffset = ");
      Serial.println((uint32_t)tmptimeoffset);
      return (seconds() - tmptimeoffset);
    }
  }
}

void showTime(uint64_t currentTime) {
  uint32_t tod = currentTime % 86400UL;
  uint8_t hh = tod / 3600;
  uint8_t mm = (tod % 3600) / 60;
  uint8_t ss = tod % 60;

  lcd.clear();
  lcd.setCursor(0, 0);

  if (hh < 10)
    lcd.print('0');
  lcd.print(hh);
  lcd.print(':');

  if (mm < 10)
    lcd.print('0');
  lcd.print(mm);
  lcd.print(':');

  if (ss < 10)
    lcd.print('0');
  lcd.print(ss);
}

int8_t waterThresholdScreen(const char *thresholdBuf, bool editMode,
                            int16_t lowThreshold, int16_t highThreshold) {
  Serial.print("[THRESH] entry: editMode=");
  Serial.print(editMode);
  Serial.print(" low=");
  Serial.print(lowThreshold);
  Serial.print(" high=");
  Serial.println(highThreshold);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(thresholdBuf);

  uint16_t low = lowThreshold;
  uint16_t high = highThreshold;
  uint8_t editing = 0;  // 0=low, 1=high
  char key = 0;
  bool modified = false;

  while (true) {
    lcd.setCursor(0, 1);
    if (editing == 0) {
      lcd.print("L:");
      if (low < 100) lcd.print('0');
      if (low < 10) lcd.print('0');
      lcd.print(low);
      lcd.print(" H:");
      if (high < 100) lcd.print('0');
      if (high < 10) lcd.print('0');
      lcd.print(high);
    } else {
      lcd.print("L:");
      if (low < 100) lcd.print('0');
      if (low < 10) lcd.print('0');
      lcd.print(low);
      lcd.print(" H:");
      if (high < 100) lcd.print('0');
      if (high < 10) lcd.print('0');
      lcd.print(high);
    }

    if (editMode) {
      // Blink cursor on current value
      if (millis() % 1000 < 500) {
        lcd.setCursor(editing == 0 ? 2 : 7, 1);
        lcd.print("   ");
      }
    }

    key = keypad.getKey();
    if (!key) {
      delay(10);
      continue;
    }

    Serial.print("[THRESH] key: ");
    Serial.println(key);

    if (key >= '0' && key <= '9') {
      if (editMode) {
        uint8_t digit = key - '0';
        if (editing == 0) {
          low = (low * 10 + digit) % 1000;
        } else {
          high = (high * 10 + digit) % 1000;
        }
        modified = true;
      }
    } else if (key == '#') {
      if (editMode) {
        editing = 1 - editing;  // Toggle between low and high
      }
    } else if (key == '*') {
      Serial.println("[THRESH] cancelled");
      return -1;
    } else if (key == 'D') {
      if (editMode) {
        if (editing == 0 && low > 0) low /= 10;
        else if (editing == 1 && high > 0) high /= 10;
        modified = true;
      }
    } else if (key == 'A') {
      if (modified) {
        // Validate thresholds
        if (low < high) {
          setWaterThresholds(low, high);
          Serial.print("[THRESH] saved - Low: ");
          Serial.print(low);
          Serial.print(", High: ");
          Serial.println(high);
          return 1;
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Error: Low >= High");
          delay(2000);
        }
      }
      break;
    }
  }

  return modified ? 1 : 0;
}
