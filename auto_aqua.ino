/**
 * ============================================================================
 * AUTO AQUA - Automatic Aquarium Management System
 * ============================================================================

 przycisk na przepompowanie przewodów od środków
**/

uint64_t lightofft = 0;
uint64_t lightont = 0;

// Function to trigger software reset via jump to address 0
// NOLINT_MANUAL_MEMORY: Low-level AVR reset mechanism
void (* const softwareReset)(void) = nullptr;

#include "debug.hpp"
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
  Serial.begin(Hardware::SERIAL_BAUD);
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
  SerialPrint(SETUP, "Language name: ", LANG_BUFFER.general.name);

  // Tank volume setup
  AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tank.volumeTitle, true, 0);
  SerialPrint(SETUP, "tankVolume = ", AppState::tankVolume);

  // Pump setup - dosing pumps only (indices 2, 3, 4)
  for (uint8_t i = 0; i < Hardware::PUMP_COUNT - 2; ++i) {
    SerialPrint(SETUP, " set pump ", i, " amount (dosing pumps only)");
    DosingConfig cfg = AppState::pumps[i].getConfig();
    cfg.amount = pumpAmountScreen(LANG_BUFFER.tank.amountTitle, i, true, 0);
    AppState::pumps[i].setConfig(cfg);
    lcd.clear();
    SerialPrint(SETUP, " pump[", i, "] amount = ", AppState::pumps[i].getConfig().amount);

    SerialPrint(SETUP, " set pump ", i, " interval (dosing pumps only)");
    cfg = AppState::pumps[i].getConfig();
    cfg.interval = pumpIntervalScreen(LANG_BUFFER.tank.intervalTitle, i, true, 0);
    AppState::pumps[i].setConfig(cfg);
    lcd.clear();
    SerialPrint(SETUP, " pump[", i, "] interval = ", AppState::pumps[i].getConfig().interval);
  }

  // Time offset and thresholds
  AppState::timeOffset = timeSetupScreen();
  SerialPrint(SETUP, " Time offset set: ", static_cast<uint32_t>(AppState::timeOffset));
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
  lcd.print(LANG_BUFFER.status.mainScreen);
  lcd.setCursor(0, 1);
  lcd.print(LANG_BUFFER.status.noTask);
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
    AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tank.volumeTitle, true,
                                            AppState::tankVolume);
    saveAppStateToConfiguration();
  } else if (k == 'C') {
    SerialPrint(LOOP, "Measuring water level");
    WaterLevelResult result = checkWaterLevel();
    displayWaterLevelStatus(result);
    delay(Hardware::UI_DELAY_LONG_MS);
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
    delay(Hardware::UI_DELAY_MEDIUM_MS);
  } else if (k == 'B') {
    SerialPrint(LOOP, "Language configuration");
    AppState::languageIndex = langConfigScreen(AppState::languageIndex);
    saveAppStateToConfiguration();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Language Set");
    delay(Hardware::UI_DELAY_MEDIUM_MS);
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
      delay(Hardware::UI_DELAY_SHORT_MS);
      softwareReset();
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
    delay(Hardware::UI_DELAY_MEDIUM_MS);
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
  delay(Hardware::UI_DELAY_SHORT_MS);
}
