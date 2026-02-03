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

  char k = keypad.getKey();
  if (k) {
    handleInputKey(k);
  }

  checkWaterLevel();
  delay(100);
}
