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

// ============================================================================
// Setup Helper Functions
// ============================================================================

void setupSerial() {
  Serial.begin(9600);
  Wire.begin();
  SerialPrint(SETUP, "Serial started");
}

void setupInitialScreen() {
  SerialPrint(SETUP, "showing splash screen");
  splashScreen();
}

void runInitialConfiguration() {
  SerialPrint(SETUP, "Configuration invalid - running setup screens");

  // Language setup
  AppState::languageIndex = langConfigScreen(0);
  SerialPrint(SETUP, "Language index set to ", AppState::languageIndex);
  LANG_BUFFER = readLanguage(AppState::languageIndex);
  SerialPrint(SETUP, " Language ", AppState::languageIndex, " loaded from PROGMEM");
  SerialPrint(SETUP, "Language name: ", LANG_BUFFER.langName);

  // Tank volume setup
  AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tankVolumeTitle, true, 0);
  SerialPrint(SETUP, "tankVolume = ", AppState::tankVolume);

  // Pump setup - dosing pumps only (indices 2, 3, 4)
  for (uint8_t i = 0; i < Hardware::PUMP_COUNT - 2; ++i) {
    SerialPrint(SETUP, " set pump ", i, " amount (dosing pumps only)");
    AppState::pumps[i].setAmount(pumpAmountScreen(LANG_BUFFER.amountTitle, i, true, 0));
    lcd.clear();
    SerialPrint(SETUP, " pump[", i, "] amount = ", AppState::pumps[i].getAmount());

    SerialPrint(SETUP, " set pump ", i, " interval (dosing pumps only)");
    AppState::pumps[i].setDosingInterval(pumpIntervalScreen(LANG_BUFFER.intervalTitle, i, true, 0));
    lcd.clear();
    SerialPrint(SETUP, " pump[", i, "] interval = ", AppState::pumps[i].getDosingInterval());
  }

  // Time offset and thresholds
  AppState::timeOffset = timeSetupScreen();
  SerialPrint(SETUP, " Time offset set: ", (uint32_t)AppState::timeOffset);
  handleThreshold();
  lightTimeScreen(lightofft, lightont);

  saveAppStateToConfiguration();
}

void loadSavedConfiguration() {
  LANG_BUFFER = readLanguage(AppState::languageIndex);
  SerialPrint(SETUP, "Language ", AppState::languageIndex, " loaded from PROGMEM");
  SerialPrint(SETUP, "Configuration loaded successfully");
}

void initializeSystem() {
  initWaterManagement();
  lcd.clear();
}

void setup() {
  setupSerial();
  setupInitialScreen();

  Configuration config = loadConfiguration();
  bool needsSetup = !isConfigurationValid(config);
  loadConfigurationToAppState();

  if (needsSetup) {
    runInitialConfiguration();
  } else {
    loadSavedConfiguration();
  }

  initializeSystem();
}

// ============================================================================
// Loop Helper Functions
// ============================================================================

void displayMainScreen() {
  lcd.setCursor(0, 0);
  lcd.print(LANG_BUFFER.mainScreen);
  lcd.setCursor(0, 1);
  lcd.print(LANG_BUFFER.noTask);
}

void handlePumpConfiguration(char k) {
  if (k >= '1' && k <= '3') {
    uint8_t pumpIndex = k - '1';
    SerialPrint(LOOP, "Editing pump amount for pump ", pumpIndex);
    handleEditAmount(pumpIndex);
    saveAppStateToConfiguration();
  }
}

void handleWaterManagement(char k) {
  if (k == 'A') {
    SerialPrint(LOOP, "Edit tank volume");
    AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tankVolumeTitle, true,
                                            AppState::tankVolume);
    saveAppStateToConfiguration();
  } else if (k == 'C') {
    SerialPrint(LOOP, "Measuring water level");
    WaterLevelResult result = checkWaterLevel();
    displayWaterLevelStatus(result);
    delay(2000);
  } else if (k == 'D') {
    SerialPrint(LOOP, "Editing tank thresholds");
    handleThreshold();
    saveAppStateToConfiguration();
  }
}

void handleSystemConfiguration(char k) {
  if (k == '0') {
    SerialPrint(LOOP, "Showing Current Time");
    showTime(seconds() + AppState::timeOffset);
    delay(1000);
  } else if (k == 'B') {
    SerialPrint(LOOP, "Language configuration");
    AppState::languageIndex = langConfigScreen(AppState::languageIndex);
    saveAppStateToConfiguration();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Language Set");
    delay(1000);
  }
}

void handleFactoryReset() {
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

void handleWaterMonitoring() {
  WaterLevelResult result = checkWaterLevel();

  if (lightont == AppState::timeOffset + seconds()) {
    digitalWrite(Hardware::LIGHT_PIN, LOW);
  } else if (lightofft == AppState::timeOffset + seconds()) {
    digitalWrite(Hardware::LIGHT_PIN, HIGH);
  }

  if (result.error != WATER_ERROR_NONE || result.inletPumpActive ||
      result.outletPumpActive) {
    displayWaterLevelStatus(result);
    delay(1000);
    lcd.clear();
  }

  checkDosingSchedule();
}

void loop() {
  displayMainScreen();

  char k = keypad.getKey();
  if (k) {
    SerialPrint(LOOP, "key -> ", k);
  }

  handlePumpConfiguration(k);
  handleWaterManagement(k);
  handleSystemConfiguration(k);

  if (k == '*') {
    handleFactoryReset();
  }

  handleWaterMonitoring();
  delay(100);
}