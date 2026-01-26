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
  if (savedTankVolume == (uint32_t)-1) {
    // Pobierz tylko potrzebne pole tankVolumeTitle
    char tankTitle[LANG_TANKTITLE_LEN + 1];
    readLanguageField(AppState::languageIndex, offsetof(Language, tankVolumeTitle), tankTitle, LANG_TANKTITLE_VISIBLE);
    int32_t tv = tankVolumeScreen(tankTitle, true, savedTankVolume);
  }
  Serial.print("[SETUP] tankVolume = ");
  Serial.println(tv);
  if (tv > 0) {
    AppState::tankVolume = (uint32_t)tv;
    saveTankVolume(AppState::tankVolume);
  }

  // Pobierz tylko potrzebne pole amountTitle
  char amountTitle[LANG_AMOUNTTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, amountTitle), amountTitle, LANG_AMOUNTTITLE_VISIBLE);

  Serial.println("[SETUP] reading amounts");
  for (uint8_t i = 0; i < PUMP_COUNT; ++i) {
    // Try to load previously saved pump amount
    uint16_t savedAmount = loadPumpAmount(i);
    // Check if not set (0xFFFF)
    uint16_t initialAmount = (savedAmount == 0xFFFF) ? 0 : savedAmount;
    int32_t v = AppState::pumps[i].edit(i, amountTitle);
    Serial.print("[SETUP] pump[");
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
  readLanguageField(AppState::languageIndex, offsetof(Language, durationTitle), durationTitle, LANG_DURATIONTITLE_VISIBLE);

  Serial.println("[SETUP] reading pump durations");
  for (uint8_t i = 0; i < PUMP_COUNT; ++i) {
    // Try to load previously saved pump duration
    uint64_t savedDuration = loadPumpDuration(i);
    // Check if not set (0xFFFFFFFFFFFFFFFF)
    uint64_t initialDuration = (savedDuration == 0xFFFFFFFFFFFFFFFFULL) ? 0 : savedDuration;
    uint64_t duration = pumpDurationScreen(durationTitle, i, true, initialDuration);
    Serial.print("[SETUP] pump[");
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

  lcd.clear();
}

namespace Screen {
void handleEditAmount(uint8_t idx) {
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.print("[AM] enter handleEditAmount idx=");
  Serial.println(idx);
  char amountTitle[LANG_AMOUNTTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, amountTitle), amountTitle, LANG_AMOUNTTITLE_VISIBLE);
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
  readLanguageField(AppState::languageIndex, offsetof(Language, tankVolumeTitle), tankTitle, LANG_TANKTITLE_VISIBLE);
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

  char durationTitle[LANG_AMOUNTTITLE_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, amountTitle), durationTitle, LANG_AMOUNTTITLE_VISIBLE);

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
  // Pobierz tylko potrzebne pola mainScreen i noTask
  char mainScreen[LANG_MAINSCREEN_LEN + 1];
  char noTask[LANG_NOTASK_LEN + 1];
  readLanguageField(AppState::languageIndex, offsetof(Language, mainScreen), mainScreen, LANG_MAINSCREEN_VISIBLE);
  readLanguageField(AppState::languageIndex, offsetof(Language, noTask), noTask, LANG_NOTASK_VISIBLE);

  lcd.setCursor(0, 0);
  lcd.print(mainScreen);
  lcd.setCursor(0, 1);
  lcd.print(noTask);

  char k = keypad.getKey();
  if (k) {
    Serial.print("[LOOP] key -> ");
    Serial.println(k);
  }

  if (k >= '1' && k <= '0' + PUMP_COUNT) {
    uint8_t idx = k - '1';
    if (idx < PUMP_COUNT) {
      Serial.print("[LOOP] dispatch amount ");
      Serial.println(idx);
      Screen::handleEditAmount(idx);
    }
  } else if (k == 'D') {
    Serial.println("[LOOP] dispatch D -> tank volume");
    Screen::handleEditTankVolume();
  } else if (k == '0') {
    Serial.println("[LOOP] Showing Current Time");
    showTime(seconds() + AppState::timeOffset);
    delay(1000);
  } else if (k == '*') {
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
  } else if (k == 'C') {  // New keybinding for water level measurement
    Serial.println("[LOOP] Measuring water level");
    long waterLevel = read_water_sensor();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Water Level:");
    lcd.setCursor(0, 1);
    lcd.print(waterLevel);
    delay(2000);                                  // Display the water level for 2 seconds
  } else if (k >= 'A' && k < 'A' + PUMP_COUNT) {  // A-E for pump duration (depending on PUMP_COUNT)
    Serial.println("[LOOP] Editing pump duration");
    uint8_t pumpIndex = k - 'A';
    Screen::handleEditPumpDuration(pumpIndex);
  }

  delay(100);
}
