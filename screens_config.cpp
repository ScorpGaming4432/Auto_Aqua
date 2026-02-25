/**
 * ============================================================================
 * SCREENS_CONFIG.CPP - Configuration Screens Implementation
 * ============================================================================
 */

#include "appstate.h"
#include "debug.hpp"
#include "display.h"
#include "language.h"
#include "screens.h"
#include "storage.h"
#include <Arduino.h>
#include <stddef.h>

/**
 * Display language selection screen
 */
uint8_t langConfigScreen(uint8_t oldLanguageIndex) {
  lcd.clear();

  char langName[LANG_NAME_LEN];
  char langPrompt[LANG_PROMPT_LEN];

  // Use offsetof for robust PROGMEM access
  readLanguageField(oldLanguageIndex, offsetof(Language, general.name), langName, LANG_NAME_LEN);
  readLanguageField(oldLanguageIndex, offsetof(Language, general.prompt), langPrompt,
                    LANG_PROMPT_LEN);
  SerialPrint(CONFIG, "Loaded language fields ", langName, " ; ", langPrompt);

  lcdPrintWithGlyphs(langName, 16, 0, 0);
  lcd.setCursor(0, 1);
  lcd.print("Num=");
  lcdPrintWithGlyphs(langPrompt, 9, 4, 1);
  lcd.setCursor(12, 1);
  lcd.print(" #->");

  uint8_t newlang = oldLanguageIndex;
  uint8_t prevlang = oldLanguageIndex;
  while (true) {
    extern void handleWaterMonitoring(bool);
    handleWaterMonitoring(false);

    char key = keypad.getKey();

    if (!key) {
      delay(10);
      continue;
    };

    if (key == '#')
      return newlang;
    if (key == '*')
      return oldLanguageIndex;

    if (key >= '0' && key <= '9')
      newlang = key - '0';
    else if (key == 'A')
      newlang = (newlang + 1) % LANG_COUNT;
    else if (key == 'B')
      newlang = (newlang + LANG_COUNT - 1) % LANG_COUNT;

    if (newlang == prevlang)
      continue;
    prevlang = newlang;

    readLanguageField(newlang, offsetof(Language, general.name), langName, LANG_NAME_LEN);
    readLanguageField(newlang, offsetof(Language, general.prompt), langPrompt, LANG_PROMPT_LEN);
    SerialPrint(CONFIG, "Loaded new language fields ", langName, " ; ", langPrompt);
    lcdPrintWithGlyphs(langName, 16, 0, 0);
    lcdPrintWithGlyphs(langPrompt, 9, 4, 1);
    lcd.setCursor(12, 1);
    lcd.print(" #->");
  }
}

uint32_t tankVolumeScreen(const char* tankVolumeBuf, bool editMode, uint32_t tankVolume) {
  if (tankVolume == UNSET_U32) {
    tankVolume = 0;
    editMode = true;
  }
  return editNumberScreen(tankVolumeBuf, "<-* _______l #->", 4, 7, tankVolume, editMode, "l");
}

void handleEditTankVolume(const char* tankTitle) {
  lcd.clear();
  lcd.setCursor(0, 0);
  int32_t tv = tankVolumeScreen(tankTitle, false, AppState::tankVolume);

  if (tv > 0) {
    AppState::tankVolume = static_cast<uint32_t>(tv);
    saveAppStateToConfiguration();
  }

  unsigned long start = millis();
  char follow = 0;
  while ((millis() - start) < 2000) {
    follow = keypad.getKey();
    if (follow)
      break;
    delay(10);
  }

  if (follow == '#') {
    int32_t ntv = tankVolumeScreen(tankTitle, true, AppState::tankVolume);
    if (ntv > 0) {
      AppState::tankVolume = static_cast<uint32_t>(ntv);
      saveAppStateToConfiguration();
    }
  }
  lcd.clear();
}

void handleThreshold() {
  while (true) {
    uint16_t low =
      static_cast<uint16_t>(editNumberScreen(LANG_BUFFER.tank.lowThresholdTitle, "     ___%    #->",
                                             8, 2, AppState::lowThreshold, true, "%"));
    uint16_t high = static_cast<uint16_t>(editNumberScreen(LANG_BUFFER.tank.highThresholdTitle,
                                                           "     ___%    #->", 8, 3,
                                                           AppState::highThreshold, true, "%"));

    if (low != UNSET_U16 && high != UNSET_U16 && low > 0 && high > low && high <= 100) {
      AppState::lowThreshold = low;
      AppState::highThreshold = high;
      saveAppStateToConfiguration();
      break;
    } else if (low == UNSET_U16 || high == UNSET_U16) {
      if (AppState::lowThreshold != UNSET_U16 && AppState::highThreshold != UNSET_U16) {
        break;
      }
    }
    delay(100);
  }
}
