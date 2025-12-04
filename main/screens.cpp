#include "language.h"
#include "display.h"
#include "keypad_config.h"
#include "utils.h"
#include "chars.h"

#include <Arduino.h>
#include <stdio.h>

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void lcdPrintWithGlyphs(const char* str, uint8_t length) {
  for (uint8_t i = 0; i < length; i++) {
    lcd->write(str[i]);
  }
}

void splashScreen() {
  loadCharSet(dropChars, 4, 0);
  // LOAD DROP ICON HERE!!!!

  lcd->clear();
  lcd->setCursor(3, 0);
  lcd->print("AUTO");
  lcd->setCursor(4, 1);
  lcd->print("AQUA");

  lcd->setCursor(12, 0); lcd->write(0);
  lcd->setCursor(11, 1); lcd->write(1);lcd->write(2);lcd->write(3);
  
}

uint8_t langConfigScreen(uint8_t idx) {
  uint8_t currentlang = idx;  // default to current language
  uint8_t newlang = currentlang;

  // Draw initial UI once
  lcd->clear();
  loadLanguageGlyphs(currentlang);
  Language tmplang = readLanguage(currentlang);
  lcd->setCursor(0, 0);
  lcdPrintWithGlyphs(tmplang.name, LANG_NAME_LEN);
  lcd->setCursor(0, 1);
  lcdPrintWithGlyphs(tmplang.prompt, LANG_PROMPT_LEN);
  lcd->print("  #->");

  // Simple language selector:
  // - Press digit 0-9 to jump to that language index (if available)
  // - Press 'A' to advance, 'B' to go back
  // - Press '#' to confirm selection
  // - Press '*' to cancel (keep previous language)
  while (true) {
    char key = keypad.getKey();

    // Process keys only when pressed
    if (key) {
      if (key >= '0' && key <= '9') {
        newlang = key - '0';
      } else if (key == 'A') {
        newlang = (currentlang + 1) % LANG_COUNT;
      } else if (key == 'B') {
        newlang = (currentlang + LANG_COUNT - 1) % LANG_COUNT;
      } else if (key == '#') {
        return currentlang;
      } else if (key == '*') {
        return languageIndex;
      }

      // If selection changed, update display and glyphs
      if (newlang != currentlang) {
        currentlang = newlang;
        lcd->clear();
        loadLanguageGlyphs(currentlang);
        tmplang = readLanguage(currentlang);
        lcd->setCursor(0, 0);
        lcdPrintWithGlyphs(tmplang.name, LANG_NAME_LEN);
        lcd->setCursor(0, 1);
        lcdPrintWithGlyphs(tmplang.prompt, LANG_PROMPT_LEN);
        lcd->print("  #->");
      }
    }

    delay(100);  // simple debounce / idle wait
  }
}


uint32_t tankVolumeScreen(const char* tankVolumeBuf) {
  uint8_t t = 0;
  uint32_t tankVolume = 0;

  lcd->clear();
  lcd->setCursor(0, 0);
  // Use fixed-length constant because language strings are stored in fixed-size
  // arrays and may not be null-terminated. Avoid relying on strlen.
  lcdPrintWithGlyphs(tankVolumeBuf, LANG_TANKTITLE_LEN);
  lcd->setCursor(0, 1);
  lcd->print("<-* _______l #->");

  while (1) {
    char key = keypad.getKey();
    if (!key) {
      lcd->setCursor(digitCount(tankVolume) + 4, 1);
      if (t) {
        lcd->print(" ");
        t--;
      } else {
        lcd->print("|");
        t++;
      }
    }

    // Cofanie (*)
    if (key == '*') {
      tankVolume = 0;

      // Wyświetl aktualny stan
      lcd->setCursor(4, 1);
      lcd->print("_______");  // czyść
      continue;
    }
    if (key == '#') {
      Serial.print("tankVolume=");
      Serial.println(tankVolume);
      return tankVolume;
    }

    // Cyfry 0-9
    if (key >= '0' && key <= '9') {
      if (tankVolume / 1000000 == 0) {
        tankVolume = (tankVolume * 10) + (key - '0');

        lcd->setCursor(4, 1);
        lcd->print("_____");  // wyczyść poprzedni tekst
        lcd->setCursor(4, 1);
        lcd->print(tankVolume);
      }
    }
  }

  return -1;  // unreachable
}