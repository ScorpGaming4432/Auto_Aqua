/**
 * ============================================================================
 * SCREENS_TIME.CPP - Time Setup Screen Implementation
 * ============================================================================
 */

#include "screens.h"
#include "display.h"
#include "storage.h"
#include <Arduino.h>

uint64_t timeSetupScreen(const char *label) {
  uint64_t nowSecs = seconds();
  uint32_t tod = (uint32_t)(nowSecs % 86400ULL);
  uint8_t hh = tod / 3600;
  uint8_t mm = (tod % 3600) / 60;
  uint8_t ss = tod % 60;

  auto showTimeFunc = [&](const char digits[6]) {
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
    lcd.print(" ");
    lcd.print(label);
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

  showTimeFunc(digits);

  uint8_t pos = 0;
  unsigned long lastBlink = millis();
  bool showCursor = true;

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
      return UNSET_U64;
    }
    if (key == '#') {
      uint8_t nh = (digits[0] - '0') * 10 + (digits[1] - '0');
      uint8_t nm = (digits[2] - '0') * 10 + (digits[3] - '0');
      uint8_t ns = (digits[4] - '0') * 10 + (digits[5] - '0');
      if (nh > 23) nh = nh % 24;
      if (nm > 59) nm = nm % 60;
      if (ns > 59) ns = ns % 60;

      uint32_t enteredSeconds = static_cast<uint32_t>(nh) * 3600UL + 
                                static_cast<uint32_t>(nm) * 60UL + 
                                static_cast<uint32_t>(ns);
      uint64_t tmptimeoffset = seconds() - static_cast<uint64_t>(enteredSeconds);
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

  if (hh < 10) lcd.print('0');
  lcd.print(hh);
  lcd.print(':');
  if (mm < 10) lcd.print('0');
  lcd.print(mm);
  lcd.print(':');
  if (ss < 10) lcd.print('0');
  lcd.print(ss);
}

void lightTimeScreen(uint64_t *lightofftime, uint64_t *lightontime) {
  lcd.clear();
  *lightofftime = timeSetupScreen("LightOFF");
  *lightontime = timeSetupScreen("LightON");
}
