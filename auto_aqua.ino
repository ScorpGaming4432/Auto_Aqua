/**
 * ============================================================================
 * AUTO AQUA - Automatic Aquarium Management System
 * ============================================================================

 przycisk na przepompowanie przewodów od środków
**/

// Function to trigger software reset via jump to address 0
// NOLINT_MANUAL_MEMORY: Low-level AVR reset mechanism
void (*const softwareReset)(void) = nullptr;

#include "appstate.h"
#include "debug.hpp"
#include "display.h"
#include "hardware.h"
#include "language.h"
#include "pumps.h"
#include "screens.h"
#include "storage.h"
#include "water.h"

// External references
extern Language LANG_BUFFER; // Defined in screens.cpp

// ============================================================================
// Setup Helper Functions
// ============================================================================

void setupSerial() {
  Serial.begin(Hardware::SERIAL_BAUD);
  Wire.begin();
  SerialPrint(SETUP, "Serial interface started @ ", Hardware::SERIAL_BAUD,
              " baud; I2C bus initialized");
}

void setupInitialScreen() {
  SerialPrint(SETUP, "Displaying splash screen before initialization");
  splashScreen();
}

void runInitialConfiguration() {
  SerialPrint(CONFIG, "Configuration missing/invalid; entering first-run setup wizard");

  // Language setup
  AppState::languageIndex = langConfigScreen(1);
  SerialPrint(CONFIG, "Language index selected by user: ", AppState::languageIndex);
  readLanguage(AppState::languageIndex, &LANG_BUFFER);
  SerialPrint(CONFIG, "Loaded language pack from PROGMEM: index=", AppState::languageIndex);
  SerialPrint(CONFIG, "Language name: ", LANG_BUFFER.general.name);

  // Tank volume setup
  AppState::tankVolume = tankVolumeScreen(LANG_BUFFER.tank.volumeTitle, true, 0);
  SerialPrint(CONFIG, "Tank volume configured to ", AppState::tankVolume, " liters");

  // Pump setup - dosing pumps only (indices 2, 3, 4)
  for (uint8_t i = 0; i < Hardware::DOSING_PUMP_COUNT; ++i) {
    SerialPrint(CONFIG, "Prompting dosing pump ", i, " amount configuration");
    DosingConfig cfg = AppState::pumps[i].getConfig();
    cfg.amount = pumpAmountScreen(LANG_BUFFER.tank.amountTitle, i, true, 0);
    AppState::pumps[i].setConfig(cfg);
    lcd.clear();
    SerialPrint(CONFIG, "Dosing pump ", Hardware::DOSING_PUMP_PINS[i],
                " amount saved: ", AppState::pumps[i].getConfig().amount, " ml");

    SerialPrint(CONFIG, "Calculated pump duration: ", 
     cfg.amount, "ml = ", calculatePumpDuration(0, cfg.amount), "ms");
    
    lcdPrintWithGlyphs(LANG_BUFFER.status.pumpWorking, LANG_PUMPWORKING_LEN, 0, 1);
    runPumpSafely(Hardware::DOSING_PUMP_PINS[i], calculatePumpDuration(0, cfg.amount));

    SerialPrint(CONFIG, "Prompting dosing pump ", i, " interval configuration");
    cfg = AppState::pumps[i].getConfig();
    cfg.interval = pumpIntervalScreen(LANG_BUFFER.tank.intervalTitle, i, true, 0);
    AppState::pumps[i].setConfig(cfg);
    lcd.clear();
    SerialPrint(CONFIG, "Dosing pump ", Hardware::DOSING_PUMP_PINS[i], " interval saved: every ",
                AppState::pumps[i].getConfig().interval, " hours");
  }

  // Time offset and thresholds
  AppState::timeOffset = timeSetupScreen();
  SerialPrint(CONFIG,
              "Clock offset configured (seconds): ", static_cast<uint32_t>(AppState::timeOffset));
  handleThreshold();

  // cleaning interval configuration (days)
  AppState::waterCleaningIntervalDays = cleanIntervalScreen(
      LANG_BUFFER.tank.cleanIntervalTitle, true, 0);
  // record the time when this configuration was performed so the first cycle will
  // occur after the specified interval has elapsed
  AppState::lastCleaningTime = AppState::timeOffset + seconds();
  SerialPrint(CONFIG, "Cleaning interval configured to ", AppState::waterCleaningIntervalDays, " days");

  lightTimeScreen(&AppState::lightOffTime, &AppState::lightOnTime);

  saveAppStateToConfiguration();
}

void loadSavedConfiguration() {
  readLanguage(AppState::languageIndex, &LANG_BUFFER);
  SerialPrint(CONFIG, "Loaded persisted language index ", AppState::languageIndex, " from PROGMEM");
  SerialPrint(CONFIG, "Persisted configuration loaded and applied");
}

void initializeSystem() {
  // Initialize light control pin
  pinMode(Hardware::LIGHT_PIN, OUTPUT);
  digitalWrite(Hardware::LIGHT_PIN, HIGH); // Start with light OFF
  SerialPrint(SETUP, "Light control pin initialized (pin ", Hardware::LIGHT_PIN, ")");

  initWaterManagement();
  SerialPrint(SETUP, "Water management subsystem initialized");
  lcd.clear();
}

void setup() {
  setupSerial();
  setupInitialScreen();

  Configuration config = loadConfiguration();
  bool needsSetup = !isConfigurationValid(config);
  SerialPrint(CONFIG,
              "Configuration validity check result: needsSetup=", needsSetup ? "true" : "false");
  loadConfigurationToAppState();

  initializeSystem();

  if (needsSetup) {
    runInitialConfiguration();
  } else {
    loadSavedConfiguration();
  }
}

// ============================================================================
// Loop Helper Functions
// ============================================================================

void displayMainScreen() {
  lcdPrintWithGlyphs(LANG_BUFFER.status.mainScreen, LANG_MAINSCREEN_LEN, 0, 0);
  lcdPrintWithGlyphs(LANG_BUFFER.status.noTask, LANG_NOTASK_LEN, 0, 1);
}

void handlePumpConfiguration(char k) {
  if (k >= '1' && k <= '3') {
    uint8_t pumpIndex = k - '1';
    SerialPrint(CONFIG, "User requested dosing amount edit for pump index ", pumpIndex);
    handleEditAmount(pumpIndex);
    saveAppStateToConfiguration();
  } else if (k >= '4' && k <= '6') {
    uint8_t pumpIndex = k - '4';
    SerialPrint(CONFIG, "User requested manual pump control for pump index ", pumpIndex);
    handleManualPumpControl(pumpIndex);
  }
}

void handleWaterManagement(char k) {
  if (k == 'A') {
    SerialPrint(CONFIG, "User requested tank volume edit");
    AppState::tankVolume =
      tankVolumeScreen(LANG_BUFFER.tank.volumeTitle, true, AppState::tankVolume);
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
  } else if (k == '7') {
    SerialPrint(CONFIG, "User requested cleaning interval edit");
    AppState::waterCleaningIntervalDays = cleanIntervalScreen(
        LANG_BUFFER.tank.cleanIntervalTitle, true,
        AppState::waterCleaningIntervalDays);
    saveAppStateToConfiguration();
  } else if (k == '#') {
    SerialPrint(CONFIG, "Manual water cleaning requested by user");
    runWaterCleaningCycle();
    AppState::lastCleaningTime = AppState::timeOffset + seconds();
    saveAppStateToConfiguration();
  }
}

void handleSystemConfiguration(char k) {
  if (k == '0') {
    SerialPrint(TIME, "Displaying current adjusted time");
    showTime(seconds() + AppState::timeOffset);
    delay(Hardware::UI_DELAY_MEDIUM_MS);
  } else if (k == '8') {
    SerialPrint(CONFIG, "User requested light time configuration");
    lightTimeScreen(&AppState::lightOffTime, &AppState::lightOnTime);
    saveAppStateToConfiguration();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Light Time Set");
    delay(Hardware::UI_DELAY_MEDIUM_MS);
  } else if (k == '9') {
    SerialPrint(TIME, "Manual override of light state requested by user");

    // Toggle light state and activate override mode
    AppState::lightState = (AppState::lightState == HIGH) ? LOW : HIGH;
    AppState::lightOverrideActive = true;
    digitalWrite(Hardware::LIGHT_PIN, AppState::lightState);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(AppState::lightState == LOW ? "Light ON" : "Light OFF");
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

// Light State Handler
// ============================================================================
// Calculates and applies light state based on current time and configured
// on/off times, unless a manual override is active. Light is controlled by
// LIGHT_PIN (pin 5):
// - LOW (0) = Light is ON
// - HIGH (1) = Light is OFF
void handleLightState() {
  // If override is active, don't recalculate; just maintain the current state
  if (AppState::lightOverrideActive) {
    digitalWrite(Hardware::LIGHT_PIN, AppState::lightState);
    return;
  }

  uint64_t currentTime = AppState::timeOffset + seconds();
  uint8_t lightState = HIGH; // Default: Light OFF

  // Handle the case where light schedule spans midnight (lightOnTime > lightOffTime)
  if (AppState::lightOnTime > AppState::lightOffTime) {
    // Light is ON from lightOnTime to 23:59:59 and from 00:00:00 to lightOffTime
    if (currentTime >= AppState::lightOnTime || currentTime <= AppState::lightOffTime) {
      lightState = LOW; // Light ON
    }
  } else
    // Normal case: light is ON from lightOnTime to lightOffTime
    if (currentTime >= AppState::lightOnTime && currentTime <= AppState::lightOffTime) {
      lightState = LOW; // Light ON
    } else {
      lightState = HIGH; // Light OFF
    }

  // Apply the light state
  digitalWrite(Hardware::LIGHT_PIN, lightState);

  // Display status only when state changes
  if (lightState != AppState::lightState) {
    AppState::lightState = lightState; 
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(lightState == LOW ? "Light ON" : "Light OFF");
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
    // handleWaterMonitoring(false);
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

void handleWaterMonitoring(bool updateDisplay) {
  // automatic cleaning scheduling based on interval in days
  if (AppState::waterCleaningIntervalDays > 0) {
    uint64_t now = AppState::timeOffset + seconds();
    uint64_t intervalSecs = static_cast<uint64_t>(AppState::waterCleaningIntervalDays) * 86400ULL;
    if (now - AppState::lastCleaningTime >= intervalSecs) {
      SerialPrint(CONFIG, "Automatic water cleaning interval reached");
      runWaterCleaningCycle();
      AppState::lastCleaningTime = now;
      saveAppStateToConfiguration();
    }
  }

  WaterLevelResult result = checkWaterLevel();

  // Rewrite the following code block to use a more descriptive and concise logging mechanism.
  if (updateDisplay &&
      (result.error != WATER_ERROR_NONE || result.inletPumpActive || result.outletPumpActive)) {
    Serial.print("Water level check result - Level: ");
    Serial.print(result.level, DEC);
    Serial.print("%; Error: ");
    if (result.error != WATER_ERROR_NONE) {
      Serial.print(result.error);
    } else {
      Serial.println("None");
    }
    Serial.print("; Inlet Pump: ");
    Serial.print(result.inletPumpActive ? "Active" : "Inactive");
    Serial.print("; Outlet Pump: ");
    Serial.print(result.outletPumpActive ? "Active" : "Inactive");

    // Instead of using delay(), consider using a more efficient and reliable way to update the
    // display.
    displayWaterLevelStatus(result);
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

  handleWaterMonitoring(true);
  handleLightState();
  delay(Hardware::UI_DELAY_SHORT_MS);
}
