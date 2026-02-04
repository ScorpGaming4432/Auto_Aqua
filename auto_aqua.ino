/**
 * ============================================================================
 * AUTO AQUA - Automatic Aquarium Management System
 * ============================================================================
 * 
 * Main Arduino sketch for an automated aquarium water management system.
 * Features:
 *   - Multi-language support (10 languages)
 *   - LCD display interface (16x2 I2C)
 *   - 4x4 keypad for user input
 *   - Multiple pump control (up to 5 pumps)
 *   - Water level sensing via I2C
 *   - Configurable tank volume and liquid amounts
 *   - Pump duration settings
 *   - Time synchronization
 *
 * Hardware:
 *   - Arduino Mega
 *   - LiquidCrystal_I2C display (0x27)
 *   - 4x4 Matrix Keypad
 *   - I2C Water level sensors
 * 
 * ============================================================================
**/

#include "screens.h"
#include "display.h"
#include "language.h"
#include "waterlevelsensor.h"
#include "pumps.h"
#include "storage.h"
#include "water.h"
#include "appstate.h"
#include "input.h"


void setup() {
  Serial.begin(9600);
  Wire.begin();  // start I2C
  Serial.println("[SETUP] Serial started");

  Serial.println("[SETUP] showing splash screen");
  splashScreen();
  Serial.println("[SETUP] running language config");

  // Try to load previously saved language index
  uint8_t savedLanguageIndex = loadLanguageIndex();
  AppState::languageIndex = langConfigScreen(savedLanguageIndex, true);
  saveLanguageIndex(AppState::languageIndex);
  Serial.print("[SETUP] languageIndex = ");
  Serial.println(AppState::languageIndex);

  Serial.println("[SETUP] reading tank volume");
  // Try to load previously saved tank volume
  uint32_t savedTankVolume = loadTankVolume();
  int32_t tv;
  if (savedTankVolume == (uint32_t)-1) {
    // Pobierz tylko potrzebne pole tankVolumeTitle
    char tankTitle[LANG_TANKTITLE_LEN + 1];
    readLanguageField(AppState::languageIndex, offsetof(Language, tankVolumeTitle), tankTitle, LANG_TANKTITLE_LEN);
    tv = tankVolumeScreen(tankTitle, true, savedTankVolume);
  } else {
    tv = savedTankVolume;
  }
  Serial.print("[SETUP] tankVolume = ");
  Serial.println(tv);
  if (tv > 0) {
    AppState::tankVolume = (uint32_t)tv;
    saveTankVolume(AppState::tankVolume);
  }

  // Pobierz tylko potrzebne pole amountTitle
  char amountTitle[LANG_AMOUNTTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, amountTitle), amountTitle, LANG_AMOUNTTITLE_LEN);

  Serial.println("[SETUP] reading amounts (dosing pumps only)");
  for (uint8_t i = 0; i < 3; ++i) {  // Only configure first 3 pumps (dosing pumps)
    // Try to load previously saved pump amount
    uint16_t savedAmount = loadPumpAmount(i);
    // Check if not set (0xFFFF)
    if (savedAmount != 0xFFFF) {
      AppState::pumps[i].setAmount(savedAmount);
    }
    int32_t v = AppState::pumps[i].edit(i, amountTitle);
    Serial.print("[SETUP] dosing pump[");
    Serial.print(i);
    Serial.print("] = ");
    Serial.println(v);
    if (v >= 0) {  // Changed: allow 0 as valid value
      AppState::pumps[i].setAmount((uint16_t)v);
      savePumpAmount(i, (uint16_t)v);
    }
  }

  // Pobierz tylko potrzebne pole durationTitle
  char durationTitle[LANG_DURATIONTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, durationTitle), durationTitle, LANG_DURATIONTITLE_LEN);

  Serial.println("[SETUP] reading pump durations (dosing pumps only)");
  for (uint8_t i = 0; i < 3; ++i) {  // Only configure first 3 pumps (dosing pumps)
    // Try to load previously saved pump duration
    uint64_t savedDuration = loadPumpDuration(i);
    // Check if not set (0xFFFFFFFFFFFFFFFF)
    uint64_t initialDuration = (savedDuration == 0xFFFFFFFFFFFFFFFFULL) ? 0 : savedDuration;
    uint64_t duration = pumpDurationScreen(durationTitle, i, true, initialDuration);
    Serial.print("[SETUP] dosing pump[");
    Serial.print(i);
    Serial.print("] duration = ");
    Serial.println((unsigned long)duration);
    if (duration != (uint64_t)-1) {
      AppState::pumps[i].setDuration(duration);
      savePumpDuration(i, duration);
    }
  }

  AppState::timeOffset = timeSetupScreen();
  saveTimeOffset(AppState::timeOffset);

  // Initialize water management system
  initWaterManagement();

  lcd.clear();
}

void loop() {
  char mainScreen[LANG_MAINSCREEN_LEN + 1];
  char noTask[LANG_NOTASK_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, mainScreen), mainScreen, LANG_MAINSCREEN_LEN);
  readLanguageField(AppState::languageIndex, offsetof(Language, noTask), noTask, LANG_NOTASK_LEN);

  lcd.setCursor(0, 0);
  lcd.print(mainScreen);
  lcd.setCursor(0, 1);
  lcd.print(noTask);

  char key = keypad.getKey();

  if (key) {

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
    else if (key == '9') {
      Serial.println("[INPUT] measure water level");
      uint8_t highBuf[12] = { 0 };
      uint8_t lowBuf[8] = { 0 };
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
    else if (key == '8') {
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
          setup();  // Re-run setup after factory reset
          break;
        } else if (confirmKey == '*') {
          Serial.println("[INPUT] factory reset cancelled");
          break;
        }
        delay(10);
      }
    }
  }


  checkWaterLevel();
  delay(100);
}
