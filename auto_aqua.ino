//TODO: Wyjebać amount z pompek bo są nie ważne i essa.

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

#include "debug.h"
#include "screens.h"
#include "display.h"
#include "language.h"
#include "water.h"
#include "pumps.h"
#include "storage.h"
#include "appstate.h"
#include "input.h"

Language LANG_BUFFER;  // Buffer to hold language data loaded from PROGMEM into RAM


void setup() {
  Serial.begin(9600);
  Wire.begin();  // start I2C

  SerialPrint("[SETUP] Serial started");

  SerialPrint("[SETUP] showing splash screen");
  splashScreen();
  StorageState configStatus = checkAllConfiguration();

  if (!configStatus.languageIndexSet) {
    SerialPrint("[SETUP] Language index not set - entering language configuration");
    AppState::languageIndex = langConfigScreen(0);
    saveLanguageIndex(AppState::languageIndex);

    SerialPrint("[SETUP] Language index set to ", AppState::languageIndex);
  }
  AppState::languageIndex = loadLanguageIndex();
  SerialPrint("[SETUP] Loaded language index: ", AppState::languageIndex);

  LANG_BUFFER = readLanguage(AppState::languageIndex);  // Load selected language into memory
  SerialPrint("[SETUP] Language ", AppState::languageIndex, " loaded from PROGMEM");


  if (!configStatus.tankVolumeSet) {
    Serial.println("[SETUP] Tank volume not set - entering tank volume configuration");
    AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tankVolumeTitle, true, 0);
  }
  saveTankVolume(AppState::tankVolume);
  SerialPrint("[SETUP] tankVolume = ", AppState::tankVolume);


  for (uint8_t i = 0; i < PUMP_COUNT - 2; ++i) {
    if (!configStatus.pumpDurationSet[i] || !configStatus.pumpAmountSet[i]) {
      SerialPrint("[SETUP] Pump", i, " not set correctly - entering pump configuration");
      SerialPrint("[SETUP] set pump ", i, " duration (dosing pumps only)");

      AppState::pumps[i].setDuration(pumpDurationScreen(LANG_BUFFER.durationTitle, i, true, 0));
      savePumpDuration(i, AppState::pumps[i].getDuration());

      SerialPrint("[SETUP] pump ", i, " duration = ", (unsigned long)AppState::pumps[i].getDuration());

      SerialPrint("[SETUP] set pump ", i, " amount (dosing pumps only)");

      AppState::pumps[i].setAmount(pumpAmountScreen(LANG_BUFFER.amountTitle, i, true, 0));
      savePumpAmount(i, AppState::pumps[i].getAmount());

      SerialPrint("[SETUP] pump[", i, "] amount = ", AppState::pumps[i].getAmount());
    } else {
      // Load existing configuration into AppState
      AppState::pumps[i].setDuration(loadPumpDuration(i));
      AppState::pumps[i].setAmount(loadPumpAmount(i));
      SerialPrint("[SETUP] Loaded pump[", i, "] duration = ", (unsigned long)AppState::pumps[i].getDuration());
      SerialPrint("[SETUP] Loaded pump[", i, "] amount = ", AppState::pumps[i].getAmount());
    }
  }

  if (!configStatus.timeOffsetSet) {
    SerialPrint("[SETUP] Time offset not set - entering time setup");
    AppState::timeOffset = timeSetupScreen();
  } else {
    AppState::timeOffset = loadTimeOffset();
    SerialPrint("[SETUP] Loaded time offset: ", AppState::timeOffset);
  }
  saveTimeOffset(AppState::timeOffset);

  // Initialize water management system
  initWaterManagement();

  lcd.clear();
}

void loop() {
  /*
   * KEY MAPPINGS:
   * 
   * === PUMP CONFIGURATION ===
   * 1-3: Edit dosing pump amounts (ml)
   * 4-6: Edit dosing pump durations (ms)
   * 
   * === WATER MANAGEMENT ===
   * Unused: Toggle water inlet pump (auto/manual mode)
   * Unused: Toggle water outlet pump (auto/manual mode)
   * A: Edit tank volume
   * C: Measure water level
   *
   * === WATER THRESHOLD CONFIGURATION ===
   * 7: Configure lower threshold
   * 8: Configure upper threshold
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

  lcd.setCursor(0, 0);
  lcd.print(LANG_BUFFER.mainScreen);
  lcd.setCursor(0, 1);
  lcd.print(LANG_BUFFER.noTask);

  char k = keypad.getKey();
  if (k) {
    Serial.print("[LOOP] key -> ");
    Serial.println(k);
  }

  // === PUMP CONFIGURATION KEYS (DOSING PUMPS ONLY) ===
  // Number keys (1-3): Edit dosing pump amounts
  if (k >= '1' && k <= '3') {
    uint8_t pumpIndex = k - '1';
    SerialPrint("[LOOP] Editing pump amount for pump ", pumpIndex);
    handleEditAmount(pumpIndex);
  }
  // Letter keys (4-6): Edit dosing pump durations
  else if (k >= '4' && k <= '6') {
    SerialPrint("[LOOP] Editing pump duration for pump ", k - '4');
    uint8_t pumpIndex = k - '4';
    handleEditPumpDuration(pumpIndex);
  }
  // // === WATER MANAGEMENT KEYS ===
  // // 4 key: Toggle water inlet pump mode
  // else if (k == '4') {
  //   Serial.println("[LOOP] Toggle water inlet pump");
  //   // Inlet pump is always automatic
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("Inlet Pump:");
  //   lcd.setCursor(0, 1);
  //   lcd.print("AUTO");
  //   delay(1000);
  // }
  // // 5 key: Toggle water outlet pump mode
  // else if (k == '5') {
  //   Serial.println("[LOOP] Toggle water outlet pump");
  //   // Outlet pump is always automatic
  //   lcd.clear();
  //   lcd.setCursor(0, 0);
  //   lcd.print("Outlet Pump:");
  //   lcd.setCursor(0, 1);
  //   lcd.print("AUTO");
  //   delay(1000);
  // }
  // D key: Edit tank volume (hold for water thresholds)
  else if (k == '7') {
    SerialPrint("[LOOP]", "Edit low water threshold");
    editNumberScreen(LANG_BUFFER.lowThresholdTitle, "     ___%    #->", 8, 2, AppState::lowThreshold, true, "%");
  }
  else if (k == 'D') {
    SerialPrint("[LOOP]", "Edit high water threshold");
    while (true) {
      int16_t low = editNumberScreen(LANG_BUFFER.lowThresholdTitle, "     ___%    #->", 8, 2, AppState::lowThreshold, true, "%");;
      int16_t high = editNumberScreen(LANG_BUFFER.highThresholdTitle, "     ___%    #->", 8, 2, AppState::highThreshold, true, "%");;
      if (low > 0 && high > low && high <= 100) {
        AppState::lowThreshold = low;
        AppState::highThreshold = high;
        saveWaterThresholds(AppState::lowThreshold, AppState::highThreshold);
        break;
      } else if (low == -1 || high == -1) {
        SerialPrint("[LOOP]", "Water threshold edit cancelled.", "   Cannot allow. Try again.");
      } if (low <= 0 || high <= 0 || low >= high || high > 100) {
        SerialPrint("[LOOP]", "Calling Police. Fish killer detected. Thresholds must be: 0 < Low < High <= 100");
      
      }
      
      SerialPrint("[LOOP]", "Invalid thresholds entered.", "   Cannot allow. Try again.");
    }
    AppState::highThreshold = editNumberScreen(LANG_BUFFER.highThresholdTitle, "     ___%    #->", 8, 2, AppState::highThreshold, true, "%");
  }
  // C key: Measure water level
  else if (k == 'C') {
    Serial.println("[LOOP] Measuring water level");
    uint8_t waterLevel = calculateWaterLevel();
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
    AppState::languageIndex = langConfigScreen(AppState::languageIndex);
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