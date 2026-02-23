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

uint64_t lightofft = 0;
uint64_t lightont = 0;

void (*resetFunc)(void) = 0;

#include "debug.h"
#include "screens.h"
#include "display.h"
#include "language.h"
#include "water.h"
#include "pumps.h"
#include "storage.h"
#include "appstate.h"
#include "input.h"
#include "hardware.h"

// External references
extern Language LANG_BUFFER;  // Defined in screens.cpp


void setup() {
  Serial.begin(9600);
  Wire.begin();  // start I2C

  SerialPrint(SETUP, "Serial started");

  SerialPrint(SETUP, "showing splash screen");
  splashScreen();
  // Load configuration from EEPROM and apply to AppState

  // Check if configuration is valid, if not, run setup screens
  Configuration config = loadConfiguration();
  bool needsSetup = !isConfigurationValid(config);

  loadConfigurationToAppState();
  if (needsSetup) {
    SerialPrint(SETUP, "Configuration invalid - running setup screens");

    // Language setup
    AppState::languageIndex = langConfigScreen(0);
    SerialPrint(SETUP, "Language index set to ", AppState::languageIndex);
    // Load language after setting it
    LANG_BUFFER = readLanguage(AppState::languageIndex);
    SerialPrint(SETUP, " Language ", AppState::languageIndex, " loaded from PROGMEM");
    SerialPrint(SETUP, "Language name: ", LANG_BUFFER.langName);

    // Tank volume setup
    AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tankVolumeTitle, true, 0);
    SerialPrint(SETUP, "tankVolume = ", AppState::tankVolume);

    // Pump setup
    for (uint8_t i = 0; i < Hardware::PUMP_COUNT - 2; ++i) {  // 2, 3, 4 - dosing pumps
      SerialPrint(SETUP, " set pump ", i, " amount (dosing pumps only)");
      AppState::pumps[i].setAmount(pumpAmountScreen(LANG_BUFFER.amountTitle, i, true, 0));
      lcd.clear();
      SerialPrint(SETUP, " pump[", i, "] amount = ", AppState::pumps[i].getAmount());

      SerialPrint(SETUP, " set pump ", i, " interval (dosing pumps only)");
      AppState::pumps[i].setDosingInterval(pumpIntervalScreen(LANG_BUFFER.intervalTitle, i, true, 0));
      lcd.clear();
      SerialPrint(SETUP, " pump[", i, "] interval = ", AppState::pumps[i].getDosingInterval());
    }

    // Time offset setup
    AppState::timeOffset = timeSetupScreen();
    SerialPrint(SETUP, " Time offset set: ", (uint32_t)AppState::timeOffset);

    handleThreshold();  // Set water thresholds

    lightTimeScreen(lightofft, lightont);

    // Save the complete configuration
    saveAppStateToConfiguration();
  } else {
    // Configuration is valid, just load language
    LANG_BUFFER = readLanguage(AppState::languageIndex);
    SerialPrint(SETUP, "Language ", AppState::languageIndex, " loaded from PROGMEM");
    SerialPrint(SETUP, "Configuration loaded successfully");
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
   * 4-6: Edit dosing pump intervals (ms)
   * 
   * === WATER MANAGEMENT ===
   * Unused: Toggle water inlet pump (auto/manual mode)
   * Unused: Toggle water outlet pump (auto/manual mode)
   * A: Edit tank volume
   * C: Measure water level
   * Unused: Toggle electrovalve
   *
   * === WATER THRESHOLD CONFIGURATION ===
   * D: Configure thresholds
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
    SerialPrint(LOOP, "key -> ", k);
  }

  // === PUMP CONFIGURATION KEYS (DOSING PUMPS ONLY) ===
  // Number keys (1-3): Edit dosing pump amounts
  if (k >= '1' && k <= '3') {
    uint8_t pumpIndex = k - '1';
    SerialPrint(LOOP, "Editing pump amount for pump ", pumpIndex);
    handleEditAmount(pumpIndex);
    saveAppStateToConfiguration();
  }

  // D key: Edit tank thresholds
  else if (k == 'D') {
    SerialPrint(LOOP, "Editing tank thresholds");
    handleThreshold();
    saveAppStateToConfiguration();
  }
  // C key: Measure water level
  else if (k == 'C') {
    SerialPrint(LOOP, "Measuring water level");
    WaterLevelResult result = checkWaterLevel();
    displayWaterLevelStatus(result);
    delay(2000);  // Display the water level for 2 seconds
  }
  // 9 key: Toggle electrovalve
  // else if (k == '9') { // TODO: DO NOT IMPLEMENT
  //   Serial.println("[LOOP] Toggle electrovalve");

  //   // Safety check - don't allow manual operation while pumps are active
  //   if (pumpActive) {
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("Pump Active!");
  //     lcd.setCursor(0, 1);
  //     lcd.print("Wait to finish");
  //     delay(2000);
  //   } else {
  //     bool currentState = isElectrovalveOpen();
  //     controlElectrovalve(!currentState);
  //     lcd.clear();
  //     lcd.setCursor(0, 0);
  //     lcd.print("Electrovalve:");
  //     lcd.setCursor(0, 1);
  //     lcd.print(currentState ? "CLOSED" : "OPENED");
  //     delay(1000);
  //   }
  // }
  // === SYSTEM STATUS KEYS ===
  // 0 key: Show current time
  else if (k == '0') {
    SerialPrint(LOOP, "Showing Current Time");
    showTime(seconds() + AppState::timeOffset);
    delay(1000);
  }
  // === SYSTEM CONFIGURATION KEYS ===
  // B key: Change language
  else if (k == 'B') {
    SerialPrint(LOOP, "Language configuration");
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
        SerialPrint(LOOP, "Factory reset confirmed");
        factoryReset();
        delay(100);
        resetFunc();
        break;
      } else if (key == '*') {
        SerialPrint(LOOP, "Factory reset cancelled");
        break;
      }
      delay(10);
    }
  }

  // digitalWrite(2, LOW); // Testing cos this is fuycking me up
  // bool lightOn = false;
  // if ((AppState::timeOffset + seconds()) % 2 == 0 && !lightOn) {
  //   // Turn on the light at specified time (e.g., every day at 8 AM)
  //   Serial.println("[LOOP] Turning on the light");
  //   digitalWrite(LIGHT_PIN, LOW);
  //   lightOn = true;
  // } else {
  //   // Turn off the light when not true (e.g., every day at 8 PM)
  //   Serial.println("[LOOP] Turning off the light");
  //   digitalWrite(LIGHT_PIN, HIGH);
  //   lightOn = false;
  // }

  // Check water level periodically for automatic pump control
  WaterLevelResult result = checkWaterLevel();

  if (lightont == AppState::timeOffset + seconds()) {
    digitalWrite(Hardware::LIGHT_PIN, LOW); // Turn on the light
  } else if (lightofft == AppState::timeOffset + seconds()) {
    digitalWrite(Hardware::LIGHT_PIN, HIGH); // Turn off the light
  }

  // Display water level status if there's an error or pumps are active
  if (result.error != WATER_ERROR_NONE || result.inletPumpActive || result.outletPumpActive) {
    displayWaterLevelStatus(result);
    delay(1000);  // Display for 1 second
    lcd.clear();
  }

  // Check dosing schedule for automatic dosing pumps
  checkDosingSchedule();


  delay(100);
}