/**
 * ============================================================================
 * SCREENS_PUMPS.CPP - Pump Configuration Screens Implementation
 * ============================================================================
 */

#include "screens.h"
#include "display.h"
#include "appstate.h"
#include "storage.h"
#include <Arduino.h>

uint16_t pumpAmountScreen(const char *amountBuf, uint8_t pumpIndex, bool editMode, uint16_t amount) {
  char _amBuf[LANG_AMOUNTTITLE_LEN + 1];
  strncpy(_amBuf, amountBuf, LANG_AMOUNTTITLE_LEN);
  _amBuf[LANG_AMOUNTTITLE_LEN] = '\0';
  for (int i = 0; _amBuf[i] != '\0'; i++) {
    if (_amBuf[i] == '#') {
      _amBuf[i] = static_cast<char>('1' + pumpIndex);
      break;
    }
  }
  if (amount == UNSET_U16) {
    amount = 0;
    editMode = true;
  }
  return static_cast<uint16_t>(editNumberScreen(_amBuf, "<-* ________ #->", 6, 6, amount, editMode, "ml"));
}

uint16_t pumpIntervalScreen(const char *intervalBuf, uint8_t pumpIndex, bool editMode, uint16_t duration) {
  char _intBuf[LANG_INTERVALTITLE_LEN + 1];
  strncpy(_intBuf, intervalBuf, LANG_INTERVALTITLE_LEN);
  _intBuf[LANG_INTERVALTITLE_LEN] = '\0';
  for (int i = 0; _intBuf[i] != '\0'; i++) {
    if (_intBuf[i] == '#') {
      _intBuf[i] = static_cast<char>('1' + pumpIndex);
      break;
    }
  }
  if (duration == UNSET_U16) {
    duration = 0;
    editMode = true;
  }
  return static_cast<uint16_t>(editNumberScreen(_intBuf, "<-* ________ #->", 6, 6, duration, editMode, "d"));
}

void handleEditAmount(uint8_t idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  int32_t v = AppState::pumps[idx].viewEdit(idx, LANG_BUFFER.tank.amountTitle);
  if (v >= 0) {
    DosingConfig cfg = AppState::pumps[idx].getConfig();
    cfg.amount = static_cast<uint16_t>(v);
    AppState::pumps[idx].setConfig(cfg);
    saveAppStateToConfiguration();
  }

  unsigned long start = millis();
  char follow = 0;
  while (millis() - start < 2000) {
    follow = keypad.getKey();
    if (follow) break;
    delay(10);
  }
  if (follow == '#') {
    int32_t nv = AppState::pumps[idx].edit(idx, LANG_BUFFER.tank.amountTitle);
    if (nv >= 0) {
      DosingConfig cfg = AppState::pumps[idx].getConfig();
      cfg.amount = static_cast<uint16_t>(nv);
      AppState::pumps[idx].setConfig(cfg);
      saveAppStateToConfiguration();
    }
  }
  lcd.clear();
}

void handleEditPumpInterval(uint8_t idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  DosingConfig cfg = AppState::pumps[idx].getConfig();
  uint32_t newInterval = pumpIntervalScreen(LANG_BUFFER.tank.intervalTitle, idx, true, static_cast<uint16_t>(cfg.interval));

  if (newInterval != UNSET_U32) {
    cfg.interval = newInterval;
    AppState::pumps[idx].setConfig(cfg);
    saveAppStateToConfiguration();
  }
  lcd.clear();
}
