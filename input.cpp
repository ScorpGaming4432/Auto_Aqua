/**
 * ============================================================================
 * INPUT.CPP - Input Handling Implementation
 * ============================================================================
 * 
 * Processes keypad input and dispatches to appropriate handlers.
 */

#include "input.h"
#include "appstate.h"
#include "screens.h"
#include "storage.h"
#include "water.h"
#include "language.h"
#include "display.h"
#include <Keypad.h>
#include <Arduino.h>

extern Keypad keypad;
extern LiquidCrystal_I2C lcd;

void handleInputKey(char key) {
  if (!key) return;

  Serial.print("[INPUT] key -> ");
  Serial.println(key);

  // === PUMP CONFIGURATION KEYS (DOSING PUMPS ONLY) ===
  // Number keys (1-3): Edit dosing pump amounts
  if (key >= '1' && key <= '3') {
    uint8_t idx = key - '1';
    Serial.print("[INPUT] edit amount pump ");
    Serial.println(idx);
    handleEditAmount(idx);
  }
  // Letter keys (A-C): Edit dosing pump durations
  else if (key >= 'A' && key <= 'C') {
    Serial.println("[INPUT] edit pump duration");
    uint8_t pumpIndex = key - 'A';
    handleEditPumpDuration(pumpIndex);
  }
  // === WATER MANAGEMENT KEYS ===
  // 4 key: Toggle water inlet pump mode
  else if (key == '4') {
    Serial.println("[INPUT] toggle inlet pump");
    toggleInletPumpMode();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Inlet Pump:");
    lcd.setCursor(0, 1);
    lcd.print(getInletPumpMode() ? "AUTO" : "MANUAL");
    delay(1000);
  }
  // 5 key: Toggle water outlet pump mode
  else if (key == '5') {
    Serial.println("[INPUT] toggle outlet pump");
    toggleOutletPumpMode();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Outlet Pump:");
    lcd.setCursor(0, 1);
    lcd.print(getOutletPumpMode() ? "AUTO" : "MANUAL");
    delay(1000);
  }
  // D key: Edit tank volume (hold for water thresholds)
  else if (key == 'D') {
    Serial.println("[INPUT] D key - checking for hold");
    unsigned long pressTime = millis();
    
    // Wait to see if key is held
    while (keypad.getKey() == 'D' && millis() - pressTime < 1000) {
      delay(10);
    }
    
    if (millis() - pressTime >= 1000) {
      // Key was held - configure water thresholds
      Serial.println("[INPUT] D key held - water thresholds");
      int8_t result = waterThresholdScreen("Water Thresh", true, 
                                           getLowThreshold(), 
                                           getHighThreshold());
      lcd.clear();
      if (result == 1) {
        lcd.print("Thresholds Set");
      } else if (result == -1) {
        lcd.print("Cancelled");
      }
      delay(1000);
    } else {
      // Quick press - edit tank volume
      Serial.println("[INPUT] D quick press - tank volume");
      handleEditTankVolume();
    }
  }
  // C key: Measure water level
  else if (key == 'C') {
    Serial.println("[INPUT] measure water level");
    uint8_t highBuf[12] = {0};
    uint8_t lowBuf[8] = {0};
    getCurrentWaterLevel(highBuf, lowBuf);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water Level OK");
    lcd.setCursor(0, 1);
    lcd.print("Check Serial");
    delay(2000);
  }
  // === SYSTEM STATUS KEYS ===
  // 0 key: Show current time
  else if (key == '0') {
    Serial.println("[INPUT] show time");
    showTime(seconds() + AppState::timeOffset);
    delay(1000);
  }
  // === SYSTEM CONFIGURATION KEYS ===
  // B key: Change language
  else if (key == 'B') {
    Serial.println("[INPUT] language config");
    AppState::languageIndex = langConfigScreen(AppState::languageIndex, true);
    saveLanguageIndex(AppState::languageIndex);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Language Set");
    delay(1000);
  }
  // === SYSTEM MAINTENANCE KEYS ===
  // * key: Factory reset
  else if (key == '*') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Factory Reset?");
    lcd.setCursor(0, 1);
    lcd.print("#=Yes  *=No");
    while (true) {
      char confirmKey = keypad.getKey();
      if (confirmKey == '#') {
        Serial.println("[INPUT] factory reset confirmed");
        factoryReset();
        delay(100);
        // Note: setup() must be called from main sketch
        break;
      } else if (confirmKey == '*') {
        Serial.println("[INPUT] factory reset cancelled");
        break;
      }
      delay(10);
    }
  }
}
