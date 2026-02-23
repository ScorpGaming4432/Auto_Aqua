/**
 * ============================================================================
 * STORAGE.CPP - Data Storage Implementation
 * ============================================================================
 *
 * Implements persistent storage of configuration data using Arduino EEPROM.
 * Handles reading/writing settings like tank volume, pump amounts, etc.
 * Uses struct-based approach to eliminate magic values.
 */
#include "debug.h"
#include "appstate.h"
#include "storage.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>

// EEPROM addresses for configuration struct
#define CONFIG_START_ADDR 0
#define CONFIG_SIZE sizeof(Configuration)

// Helper function to write bytes to EEPROM
static void writeEEPROMBytes(uint16_t address, const uint8_t* data, uint16_t size) {
  SerialPrint(STORAGE, F("Writing "), size, F(" bytes to EEPROM at address "), address);
  SerialPrint(STORAGE, F("Data: "));
  for (uint16_t i = 0; i < size; i++) {
    Serial.print(data[i], HEX);
    Serial.print(F(" "));
    EEPROM.write(address + i, data[i]);
  }
  Serial.println();
}

// Helper function to read bytes from EEPROM
static void readEEPROMBytes(uint16_t address, uint8_t* data, uint16_t size) {
  SerialPrint(STORAGE, F("Reading "), size, F(" bytes from EEPROM at address "), address);
  SerialPrint(STORAGE, F("Data: "));
  for (uint16_t i = 0; i < size; i++) {
    data[i] = EEPROM.read(address + i);
    Serial.print(data[i], HEX);
    Serial.print(F(" "));
  }
  Serial.println();
}

void saveConfiguration(const Configuration& config) {
  SerialPrint(STORAGE, F("Saving configuration to EEPROM"));

  // Write the entire configuration struct to EEPROM
  writeEEPROMBytes(CONFIG_START_ADDR, (const uint8_t*)&config, CONFIG_SIZE);

  SerialPrint(STORAGE, F("Configuration saved successfully"));
}

Configuration loadConfiguration() {
  SerialPrint(STORAGE, F("Loading configuration from EEPROM"));

  Configuration config;
  readEEPROMBytes(CONFIG_START_ADDR, (uint8_t*)&config, CONFIG_SIZE);

  SerialPrint(STORAGE, F("Configuration loaded successfully"));
  return config;
}

bool isConfigurationValid(const Configuration& config) {
  SerialPrint(STORAGE, F("Validating configuration..."));

  bool valid = true;

  if (config.languageIndex == UNSET_U8) {
    SerialPrint(STORAGE, F("Invalid: languageIndex is UNSET. "), config.languageIndex);
    valid = false;
  }

  if (config.tankVolume == UNSET_U32) {
    SerialPrint(STORAGE, F("Invalid: tankVolume is UNSET. "), config.tankVolume);
    valid = false;
  }

  if (config.timeOffset == UNSET_I64) {
    double value;
    memcpy(&value, &config.timeOffset, sizeof(double));
    SerialPrint(STORAGE, F("Invalid: timeOffset is UNSET. "), value);

    valid = false;
  }

  for (uint8_t i = 0; i < Hardware::PUMP_COUNT; i++) {
    if (config.pumpAmounts[i] == UNSET_U16) {
      SerialPrint(STORAGE, F("Invalid: pumpAmounts["), i, F("] is UNSET. "), config.pumpAmounts[i]);
      valid = false;
    }

    if (config.pumpDurations[i] == UNSET_U64) {
      double value;
      memcpy(&value, &config.pumpDurations[i], sizeof(double));

      SerialPrint(STORAGE, F("Invalid: pumpDurations["), i, F("] is UNSET. "), value);
      valid = false;
    }

    if (config.pumpDosingIntervals[i] == UNSET_U32) {
      SerialPrint(STORAGE, F("Invalid: pumpDosingIntervals["), i, F("] is UNSET. "), config.pumpDosingIntervals[i]);
      valid = false;
    }
  }

  if (config.lowThreshold > config.highThreshold) {
    SerialPrint(STORAGE, F("Invalid: Thresholds are INVALID.Low="), config.lowThreshold, F(" ; High="), config.highThreshold);
    valid = false;
  }

  if (config.lowThreshold >= 100) {
    SerialPrint(STORAGE, F("Invalid: lowThreshold is UNSET. Low="), config.lowThreshold);
    valid = false;
  }

  if (config.highThreshold > 100) {
    SerialPrint(STORAGE, F("Invalid: highThreshold is UNSET High="), config.highThreshold);
    valid = false;
  }

  if (valid) {
    SerialPrint(STORAGE, F("Configuration is valid."));
  } else {
    SerialPrint(STORAGE, F("Configuration validation FAILED."));
  }

  return valid;
}

void loadConfigurationToAppState() {
  SerialPrint(STORAGE, F("Loading configuration to AppState"));

  Configuration config = loadConfiguration();

  // Only apply valid configuration
  if (isConfigurationValid(config)) {
    AppState::languageIndex = config.languageIndex;
    AppState::tankVolume = config.tankVolume;
    AppState::timeOffset = config.timeOffset;
    AppState::lowThreshold = config.lowThreshold;
    AppState::highThreshold = config.highThreshold;

    for (uint8_t i = 0; i < Hardware::PUMP_COUNT; i++) {
      AppState::pumps[i].setAmount(config.pumpAmounts[i]);
      AppState::pumps[i].setDuration(config.pumpDurations[i]);
      AppState::pumps[i].setDosingInterval(config.pumpDosingIntervals[i]);
    }

    SerialPrint(STORAGE, F("Configuration applied to AppState"));
  } else {
    SerialPrint(STORAGE, F("Invalid configuration, using defaults"));
    // Apply default configuration
    AppState::languageIndex = DEFAULT_CONFIG.languageIndex;
    AppState::tankVolume = DEFAULT_CONFIG.tankVolume;
    AppState::timeOffset = DEFAULT_CONFIG.timeOffset;
    AppState::lowThreshold = DEFAULT_CONFIG.lowThreshold;
    AppState::highThreshold = DEFAULT_CONFIG.highThreshold;

    for (uint8_t i = 0; i < Hardware::PUMP_COUNT; i++) {
      AppState::pumps[i].setAmount(DEFAULT_CONFIG.pumpAmounts[i]);
      AppState::pumps[i].setDuration(DEFAULT_CONFIG.pumpDurations[i]);
      AppState::pumps[i].setDosingInterval(DEFAULT_CONFIG.pumpDosingIntervals[i]);
    }
  }
}

void saveAppStateToConfiguration() {
  SerialPrint(STORAGE, F("Saving AppState to configuration"));

  Configuration config;

  config.languageIndex = AppState::languageIndex;
  config.tankVolume = AppState::tankVolume;
  config.timeOffset = AppState::timeOffset;
  config.lowThreshold = AppState::lowThreshold;
  config.highThreshold = AppState::highThreshold;

  for (uint8_t i = 0; i < Hardware::PUMP_COUNT; i++) {
    config.pumpAmounts[i] = AppState::pumps[i].getAmount();
    config.pumpDurations[i] = AppState::pumps[i].getDuration();
    config.pumpDosingIntervals[i] = AppState::pumps[i].getDosingInterval();
  }

  saveConfiguration(config);
  SerialPrint(STORAGE, F("AppState saved to configuration"));
}

void factoryReset() {
  SerialPrint(STORAGE, F("==== FACTORY RESET ====="));

  // Create a configuration with magic values
  Configuration resetConfig;

  resetConfig.languageIndex = UNSET_U8;
  resetConfig.tankVolume = UNSET_U32;
  resetConfig.timeOffset = UNSET_I64;
  resetConfig.lowThreshold = UNSET_U16;
  resetConfig.highThreshold = UNSET_U16;

  for (uint8_t i = 0; i < Hardware::PUMP_COUNT; i++) {
    resetConfig.pumpAmounts[i] = UNSET_U16;
    resetConfig.pumpDurations[i] = UNSET_U64;
    resetConfig.pumpDosingIntervals[i] = UNSET_U32;
  }

  saveConfiguration(resetConfig);

  SerialPrint(STORAGE, F("Factory reset completed - all values set to unset state"));
  SerialPrint(STORAGE, F("===================================="));
}
