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

void(* resetFunc) (void) = 0;

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

  Serial.print(SETUP);Serial.println("Serial started");

  Serial.print(SETUP);Serial.println("showing splash screen");
  splashScreen();
  // Load configuration from EEPROM and apply to AppState
  loadConfigurationToAppState();

  // Check if configuration is valid, if not, run setup screens
  Configuration config = loadConfiguration();
  bool needsSetup = !isConfigurationValid(config);

  if (needsSetup) {
    Serial.print(SETUP);Serial.println(" Configuration invalid - running setup screens");

    // Language setup
    AppState::languageIndex = langConfigScreen(0);
    Serial.print(SETUP);Serial.print(" Language index set to ");Serial.println(AppState::languageIndex);
    // Load language after setting it
    LANG_BUFFER = readLanguage(AppState::languageIndex);
    Serial.print(SETUP);Serial.print(" Language ");Serial.print(AppState::languageIndex);Serial.println(" loaded from PROGMEM");
    Serial.println(LANG_BUFFER.langName);

    // Tank volume setup
    AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tankVolumeTitle, true, 0);
    Serial.print(SETUP);Serial.print("tankVolume = ");Serial.println(AppState::tankVolume);

    // Pump setup
    for (uint8_t i = 0; i < PUMP_COUNT - 2; ++i) {  // 2, 3, 4 - dosing pumps
      Serial.print(SETUP);Serial.print(" set pump ");Serial.print(i);Serial.println(" amount (dosing pumps only)");
      AppState::pumps[i].setAmount(pumpAmountScreen(LANG_BUFFER.amountTitle, i, true, 0));
      Serial.print(SETUP);Serial.print(" pump[");Serial.print(i);Serial.print("] amount = ");Serial.println(AppState::pumps[i].getAmount());
    }

    // Time offset setup
    AppState::timeOffset = timeSetupScreen();
    Serial.print(SETUP);Serial.print(" Time offset set: ");Serial.println((uint32_t) AppState::timeOffset);

    // Save the complete configuration
    saveAppStateToConfiguration();
  } else {
    // Configuration is valid, just load language
    LANG_BUFFER = readLanguage(AppState::languageIndex);
    Serial.print(SETUP);Serial.print("Language ");Serial.print(AppState::languageIndex);Serial.println(" loaded from PROGMEM");
    Serial.print(SETUP);Serial.println("Configuration loaded successfully");
  }

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
   * 9: Toggle electrovalve
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
    Serial.print("[LOOP] Editing pump amount for pump "); Serial.println(pumpIndex);
    handleEditAmount(pumpIndex);
  }

  // D key: Edit tank volume (hold for water thresholds)
  else if (k == '7') {
    Serial.println("[LOOP] Edit low water threshold");
    editNumberScreen(LANG_BUFFER.lowThresholdTitle, "     ___%    #->", 8, 2, AppState::lowThreshold, true, "%");
  } else if (k == 'D') {
    Serial.println("[LOOP] Edit high water threshold");
    while (true) {
      uint8_t low = (uint8_t)editNumberScreen(LANG_BUFFER.lowThresholdTitle, "     ___%    #->", 8, 2, AppState::lowThreshold, true, "%");
      uint8_t high = (uint8_t)editNumberScreen(LANG_BUFFER.highThresholdTitle, "     ___%    #->", 8, 3, AppState::highThreshold, true, "%");
      Serial.print("[LOOP] Entered thresholds - Low: "); Serial.print(low); Serial.print("% High: "); Serial.print(high); Serial.println("%");
      if (low > 0 && high > low && high <= 100) {
        AppState::lowThreshold = low;
        AppState::highThreshold = high;
        saveAppStateToConfiguration();
        break;
      } else if (low == UNSET_U8 || high == UNSET_U8) {
        Serial.println("[LOOP] Water threshold edit cancelled.");
      } else if (low >= high || high > 100) {
        Serial.println("[LOOP] Calling Police. Fish killer detected. Thresholds must be: 0 < Low < High <= 100");
      }

      Serial.println("[LOOP] Invalid thresholds entered. Cannot allow. Try again.");
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
  // 9 key: Toggle electrovalve
  else if (k == '9') {
    Serial.println("[LOOP] Toggle electrovalve");

    // Safety check - don't allow manual operation while pumps are active
    if (pumpActive) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Pump Active!");
      lcd.setCursor(0, 1);
      lcd.print("Wait to finish");
      delay(2000);
    } else {
      bool currentState = isElectrovalveOpen();
      controlElectrovalve(!currentState);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Electrovalve:");
      lcd.setCursor(0, 1);
      lcd.print(currentState ? "CLOSED" : "OPENED");
      delay(1000);
    }
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
    saveAppStateToConfiguration();
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
        resetFunc();
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

  // Check dosing schedule for automatic dosing pumps
  checkDosingSchedule();


  delay(100);
}