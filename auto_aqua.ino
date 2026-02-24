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
#include "hardware.h"

// External references
extern Language LANG_BUFFER;  // Defined in screens.cpp

// ============================================================================
// Setup Helper Functions
// ============================================================================

void setupSerial() {
  Serial.begin(Hardware::SERIAL_BAUD);
  Wire.begin();
  SerialPrint(SETUP, "Serial interface started @ ", Hardware::SERIAL_BAUD, " baud; I2C bus initialized");
}

void setupInitialScreen() {
  SerialPrint(SETUP, "Displaying splash screen before initialization");
  splashScreen();
}

void runInitialConfiguration() {
  SerialPrint(CONFIG, "Configuration missing/invalid; entering first-run setup wizard");

  // Language setup
  AppState::languageIndex = langConfigScreen(0);
  SerialPrint(CONFIG, "Language index selected by user: ", AppState::languageIndex);
  readLanguage(AppState::languageIndex, &LANG_BUFFER);
  SerialPrint(CONFIG, "Loaded language pack from PROGMEM: index=", AppState::languageIndex);
  SerialPrint(CONFIG, "Language name: ", LANG_BUFFER.general.name);

  // Tank volume setup
  AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tank.volumeTitle, true, 0);
  SerialPrint(CONFIG, "Tank volume configured to ", AppState::tankVolume, " liters");

  // Pump setup - dosing pumps only (indices 2, 3, 4)
  for (uint8_t i = 0; i < Hardware::PUMP_COUNT - 2; ++i) {
    SerialPrint(CONFIG, "Prompting dosing pump ", i, " amount configuration");
    DosingConfig cfg = AppState::pumps[i].getConfig();
    cfg.amount = pumpAmountScreen(LANG_BUFFER.tank.amountTitle, i, true, 0);
    AppState::pumps[i].setConfig(cfg);
    lcd.clear();
    SerialPrint(CONFIG, "Dosing pump ", i, " amount saved: ", AppState::pumps[i].getConfig().amount, " ml");

    SerialPrint(CONFIG, "Prompting dosing pump ", i, " interval configuration");
    cfg = AppState::pumps[i].getConfig();
    cfg.interval = pumpIntervalScreen(LANG_BUFFER.tank.intervalTitle, i, true, 0);
    AppState::pumps[i].setConfig(cfg);
    lcd.clear();
    SerialPrint(CONFIG, "Dosing pump ", i, " interval saved: every ", AppState::pumps[i].getConfig().interval, " hours");
  }

  // Time offset and thresholds
  AppState::timeOffset = timeSetupScreen();
  SerialPrint(CONFIG, "Clock offset configured (seconds): ", static_cast<uint32_t>(AppState::timeOffset));
  handleThreshold();
  lightTimeScreen(&lightofft, &lightont);

  saveAppStateToConfiguration();
}

void loadSavedConfiguration() {
  readLanguage(AppState::languageIndex, &LANG_BUFFER);
  SerialPrint(CONFIG, "Loaded persisted language index ", AppState::languageIndex, " from PROGMEM");
  SerialPrint(CONFIG, "Persisted configuration loaded and applied");
}

void initializeSystem() {
  initWaterManagement();
  SerialPrint(SETUP, "Water management subsystem initialized");
  lcd.clear();
}

void setup() {
  setupSerial();
  setupInitialScreen();

  Configuration config = loadConfiguration();
  bool needsSetup = !isConfigurationValid(config);
  SerialPrint(CONFIG, "Configuration validity check result: needsSetup=", needsSetup ? "true" : "false");
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
  lcdPrintWithGlyphs(LANG_BUFFER.status.mainScreen, LANG_MAINSCREEN_LEN);
  lcd.setCursor(0, 1);
  lcdPrintWithGlyphs(LANG_BUFFER.status.noTask, LANG_NOTASK_LEN);
}

void handlePumpConfiguration(char k) {
  if (k >= '1' && k <= '3') {
    uint8_t pumpIndex = k - '1';
    SerialPrint(CONFIG, "User requested dosing amount edit for pump index ", pumpIndex);
    handleEditAmount(pumpIndex);
    saveAppStateToConfiguration();
  }
}

void handleWaterManagement(char k) {
  if (k == 'A') {
    SerialPrint(CONFIG, "User requested tank volume edit");
    AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tank.volumeTitle, true,
                                            AppState::tankVolume);
    saveAppStateToConfiguration();
  } else if (k == 'C') {
    SerialPrint(MONITOR, "Manual water-level measurement requested");
    WaterLevelResult result = checkWaterLevel();
    displayWaterLevelStatus(result);
    delay(Hardware::UI_DELAY_LONG_MS);
  } else if (k == 'D') {
    SerialPrint(CONFIG, "User requested tank threshold recalibration");
    handleThreshold();
    saveAppStateToConfiguration();
  }
}

void handleSystemConfiguration(char k) {
  if (k == '0') {
    SerialPrint(TIME, "Displaying current adjusted time");
    showTime(seconds() + AppState::timeOffset);
    delay(Hardware::UI_DELAY_MEDIUM_MS);
  } else if (k == 'B') {
    SerialPrint(CONFIG, "User entered language configuration screen");
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
    handleWaterMonitoring();
    char key = keypad.getKey();
    if (key == '#') {
      SerialPrint(FACTORY, "Factory reset confirmed by user; erasing persisted config");
      factoryReset();
      delay(Hardware::UI_DELAY_SHORT_MS);
      softwareReset();
      break;
    } else if (key == '*') {
      SerialPrint(FACTORY, "Factory reset cancelled by user");
      break;
    }
    delay(10);
  }
}

void handleWaterMonitoring() {
  WaterLevelResult result = checkWaterLevel();

  if (lightont == AppState::timeOffset + seconds()) {
    SerialPrint(LIGHTS, "Light schedule hit ON timestamp; setting relay LOW (light ON)");
    digitalWrite(Hardware::LIGHT_PIN, LOW);
  } else if (lightofft == AppState::timeOffset + seconds()) {
    SerialPrint(LIGHTS, "Light schedule hit OFF timestamp; setting relay HIGH (light OFF)");
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
    SerialPrint(KEYPAD_INPUT, "Keypad event received: ", k);
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
