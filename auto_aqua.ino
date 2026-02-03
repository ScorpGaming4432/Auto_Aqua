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
#include "sensor.h"
#include "pumps.h"
#include "storage.h"
#include "water.h"


namespace AppState {
uint8_t languageIndex = 0;
Pump pumps[PUMP_COUNT];
uint32_t tankVolume = 0;
int64_t timeOffset = 0;
}


void setup() {
  Serial.begin(9600);
  Wire.begin();  // start I2C
  Serial.println("[SETUP] Serial started");

  Serial.println("[SETUP] showing splash screen");
  splashScreen();
  Serial.println("[SETUP] running language config");

  // Try to load previously saved language index
  uint8_t savedLanguageIndex = loadLanguageIndex();
  if (savedLanguageIndex == (uint8_t)-1) {
    AppState::languageIndex = langConfigScreen(savedLanguageIndex, true);
    saveLanguageIndex(AppState::languageIndex);
  };
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

namespace Screen {
void handleEditAmount(uint8_t idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.print("[AM] enter handleEditAmount idx=");
  Serial.println(idx);
  char amountTitle[LANG_AMOUNTTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, amountTitle), amountTitle, LANG_AMOUNTTITLE_LEN);
  int32_t v = AppState::pumps[idx].viewEdit(idx, amountTitle);
  Serial.print("[AM] current (view) idx=");
  Serial.print(idx);
  Serial.print(" -> ");
  Serial.println(v);
  if (v >= 0) {  // Changed: allow 0 as valid value
    AppState::pumps[idx].setAmount((uint16_t)v);
    savePumpAmount(idx, (uint16_t)v);
  }

  unsigned long start = millis();
  char follow = 0;
  while (millis() - start < 2000) {
    follow = keypad.getKey();
    if (follow) break;
    delay(10);
  }
  Serial.print("[AM] followup key for idx=");
  Serial.print(idx);
  Serial.print(" -> ");
  Serial.println(follow);
  if (follow == '#') {
    Serial.print("[AM] entering edit mode idx=");
    Serial.println(idx);
    int32_t nv = AppState::pumps[idx].edit(idx, amountTitle);
    Serial.print("[AM] edited idx=");
    Serial.print(idx);
    Serial.print(" -> ");
    Serial.println(nv);
    if (nv >= 0) {  // Changed: allow 0 as valid value
      AppState::pumps[idx].setAmount((uint16_t)nv);
      savePumpAmount(idx, (uint16_t)nv);
    }
  }
  lcd.clear();
}

void handleEditTankVolume() {
  lcd.clear();

  lcd.setCursor(0, 0);
  Serial.println("[TANK] enter handleEditTankVolume");
  char tankTitle[LANG_TANKTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, tankVolumeTitle), tankTitle, LANG_TANKTITLE_LEN);
  int32_t tv = tankVolumeScreen(tankTitle, false, AppState::tankVolume);
  Serial.print("[TANK] current (view) -> ");
  Serial.println(tv);
  if (tv > 0) {
    AppState::tankVolume = (uint32_t)tv;
    saveTankVolume(AppState::tankVolume);
  }

  unsigned long start = millis();
  char follow = 0;
  while ((millis() - start) < 2000) {
    follow = keypad.getKey();
    if (follow) break;
    delay(10);
  }
  Serial.print("[TANK] followup key -> ");
  Serial.println(follow);
  if (follow == '#') {
    Serial.println("[TANK] entering edit mode");
    int32_t ntv = tankVolumeScreen(tankTitle, true, AppState::tankVolume);
    Serial.print("[TANK] edited -> ");
    Serial.println(ntv);
    if (ntv > 0) {
      AppState::tankVolume = (uint32_t)ntv;
      saveTankVolume(AppState::tankVolume);
    }
  }
  lcd.clear();
}

void handleEditPumpDuration(uint8_t idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.print("[DUR] enter handleEditPumpDuration idx=");
  Serial.println(idx);

  // Use the correct duration title field and length
  char durationTitle[LANG_DURATIONTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, durationTitle), durationTitle, LANG_DURATIONTITLE_LEN);

  uint64_t duration = AppState::pumps[idx].getDuration();
  uint64_t newDuration = pumpDurationScreen(durationTitle, idx, true, duration);

  Serial.print("[DUR] edited idx=");
  Serial.print(idx);
  Serial.print(" -> ");
  Serial.println((unsigned long)newDuration);

  if (newDuration != (uint64_t)-1) {
    AppState::pumps[idx].setDuration(newDuration);
    savePumpDuration(idx, newDuration);
  }

  lcd.clear();
}

}  // namespace Screen

void loop() {
  /*
   * KEY MAPPINGS:
   * 
   * === PUMP CONFIGURATION ===
   * 1-3: Edit dosing pump amounts (ml)
   * A-C: Edit dosing pump durations (ms)
   * 
   * === WATER MANAGEMENT ===
   * 4: Toggle water inlet pump (auto/manual mode)
   * 5: Toggle water outlet pump (auto/manual mode)
   * D: Edit tank volume
   * C: Measure water level
   * 
   * === SYSTEM STATUS ===
   * 0: Show current time
   * 
   * === SYSTEM CONFIGURATION ===
   * B: Change language
   * 
   * === SYSTEM MAINTENANCE ===
   * *: Factory reset (confirm with #)
   */
  
  // Pobierz tylko potrzebne pola mainScreen i noTask
  char mainScreen[LANG_MAINSCREEN_LEN + 1];
  char noTask[LANG_NOTASK_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, mainScreen), mainScreen, LANG_MAINSCREEN_LEN);
  readLanguageField(AppState::languageIndex, offsetof(Language, noTask), noTask, LANG_NOTASK_LEN);

  lcd.setCursor(0, 0);
  lcd.print(mainScreen);
  lcd.setCursor(0, 1);
  lcd.print(noTask);

  char k = keypad.getKey();
  if (k) {
    Serial.print("[LOOP] key -> ");
    Serial.println(k);
  }

  // === PUMP CONFIGURATION KEYS (DOSING PUMPS ONLY) ===
  // Number keys (1-3): Edit dosing pump amounts
  if (k >= '1' && k <= '3') {
    uint8_t idx = k - '1';
    Serial.print("[LOOP] dispatch amount ");
    Serial.println(idx);
    Screen::handleEditAmount(idx);
  }
  // Letter keys (A-C): Edit dosing pump durations
  else if (k >= 'A' && k <= 'C') {
    Serial.println("[LOOP] Editing pump duration");
    uint8_t pumpIndex = k - 'A';
    Screen::handleEditPumpDuration(pumpIndex);
  }
  // === WATER MANAGEMENT KEYS ===
  // 4 key: Toggle water inlet pump mode
  else if (k == '4') {
    Serial.println("[LOOP] Toggle water inlet pump");
    toggleInletPumpMode();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Inlet Pump:");
    lcd.setCursor(0, 1);
    lcd.print(getInletPumpMode() ? "AUTO" : "MANUAL");
    delay(1000);
  }
  // 5 key: Toggle water outlet pump mode
  else if (k == '5') {
    Serial.println("[LOOP] Toggle water outlet pump");
    toggleOutletPumpMode();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Outlet Pump:");
    lcd.setCursor(0, 1);
    lcd.print(getOutletPumpMode() ? "AUTO" : "MANUAL");
    delay(1000);
  }
  // D key: Edit tank volume (hold for water thresholds)
  else if (k == 'D') {
    Serial.println("[LOOP] D key pressed - checking for hold");
    unsigned long pressTime = millis();
    bool held = false;
    
    // Wait to see if key is held
    while (keypad.getKey() == 'D' && millis() - pressTime < 1000) {
      delay(10);
    }
    
    if (millis() - pressTime >= 1000) {
      // Key was held - configure water thresholds
      Serial.println("[LOOP] D key held -> water thresholds");
      char thresholdTitle[16];
      readLanguageField(AppState::languageIndex, offsetof(Language, mainScreen), thresholdTitle, 16);
      // For now, using mainScreen as placeholder - should add proper field
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
      Serial.println("[LOOP] D quick press -> tank volume");
      Screen::handleEditTankVolume();
    }
  }
  // C key: Measure water level
  else if (k == 'C') {
    Serial.println("[LOOP] Measuring water level");
    long waterLevel = read_water_sensor();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water Level:");
    lcd.setCursor(0, 1);
    lcd.print(waterLevel);
    delay(2000);  // Display the water level for 2 seconds
  }
  // === SYSTEM STATUS KEYS ===
  // 0 key: Show current time
  else if (k == '0') {
    Serial.println("[LOOP] Showing Current Time");
    showTime(seconds() + AppState::timeOffset);
    delay(1000);
  }
  // === SYSTEM CONFIGURATION KEYS ===
  // B key: Change language
  else if (k == 'B') {
    Serial.println("[LOOP] Language configuration");
    AppState::languageIndex = langConfigScreen(AppState::languageIndex, true);
    saveLanguageIndex(AppState::languageIndex);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Language Set");
    delay(1000);
  }
  // === SYSTEM MAINTENANCE KEYS ===
  // * key: Factory reset
  else if (k == '*') {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Factory Reset?");
    lcd.setCursor(0, 1);
    lcd.print("#=Yes  *=No");
    while (true) {
      char key = keypad.getKey();
      if (key == '#') {
        Serial.println("[LOOP] factory reset confirmed");
        factoryReset();
        delay(100);
        setup();
        break;
      } else if (key == '*') {
        Serial.println("[LOOP] factory reset cancelled");
        break;
      }
      delay(10);
    }
  }

  // Check water level periodically for automatic pump control
  checkWaterLevel();

  delay(100);
}
