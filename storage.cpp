/**
 * ============================================================================
 * STORAGE.CPP - Data Storage Implementation
 * ============================================================================
 *
 * Implements persistent storage of configuration data using Arduino EEPROM.
 * Handles reading/writing settings like tank volume, pump amounts, etc.
 * Uses struct-based approach to eliminate magic values.
 */
#include "appstate.h"
#include "storage.h"
#include "language.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>

// EEPROM addresses for configuration struct
#define CONFIG_START_ADDR 0
#define CONFIG_SIZE sizeof(Configuration)

// Helper function to write bytes to EEPROM
static void writeEEPROMBytes(uint16_t address, const uint8_t* data, uint16_t size) {
  for (uint16_t i = 0; i < size; i++) {
    EEPROM.write(address + i, data[i]);
  }
}

// Helper function to read bytes from EEPROM
static void readEEPROMBytes(uint16_t address, uint8_t* data, uint16_t size) {
  for (uint16_t i = 0; i < size; i++) {
    data[i] = EEPROM.read(address + i);
  }
}

void saveConfiguration(const Configuration& config) {
  Serial.println("[STORAGE] Saving configuration to EEPROM");

  // Write the entire configuration struct to EEPROM
  writeEEPROMBytes(CONFIG_START_ADDR, (const uint8_t*)&config, CONFIG_SIZE);

  Serial.println("[STORAGE] Configuration saved successfully");
}

Configuration loadConfiguration() {
  Serial.println("[STORAGE] Loading configuration from EEPROM");

  Configuration config;
  readEEPROMBytes(CONFIG_START_ADDR, (uint8_t*)&config, CONFIG_SIZE);

  Serial.println("[STORAGE] Configuration loaded successfully");
  return config;
}

bool isConfigurationValid(const Configuration& config) {
  // Check if any field contains magic values
  if (config.languageIndex == UNSET_U8) return false;
  if (config.tankVolume == UNSET_U32) return false;
  if (config.timeOffset == UNSET_I64) return false;

  for (uint8_t i = 0; i < PUMP_COUNT; i++) {
    if (config.pumpAmounts[i] == UNSET_U16) return false;
    if (config.pumpDurations[i] == UNSET_U64) return false;
  }

  if (config.lowThreshold == UNSET_U16) return false;
  if (config.highThreshold == UNSET_U16) return false;

  return true;
}
value |= ((uint64_t)EEPROM.read(address + 1) << 8);
value |= ((uint64_t)EEPROM.read(address + 2) << 16);
value |= ((uint64_t)EEPROM.read(address + 3) << 24);
value |= ((uint64_t)EEPROM.read(address + 4) << 32);
value |= ((uint64_t)EEPROM.read(address + 5) << 40);
value |= ((uint64_t)EEPROM.read(address + 6) << 48);
value |= ((uint64_t)EEPROM.read(address + 7) << 56);
return value;
}

void loadConfigurationToAppState() {
  Serial.println("[STORAGE] Loading configuration to AppState");

  Configuration config = loadConfiguration();

  // Only apply valid configuration
  if (isConfigurationValid(config)) {
    AppState::languageIndex = config.languageIndex;
    AppState::tankVolume = config.tankVolume;
    AppState::timeOffset = config.timeOffset;
    AppState::lowThreshold = config.lowThreshold;
    AppState::highThreshold = config.highThreshold;

    for (uint8_t i = 0; i < PUMP_COUNT; i++) {
      AppState::pumps[i].setAmount(config.pumpAmounts[i]);
      AppState::pumps[i].setDuration(config.pumpDurations[i]);
    }

    Serial.println("[STORAGE] Configuration applied to AppState");
  } else {
    Serial.println("[STORAGE] Invalid configuration, using defaults");
    // Apply default configuration
    AppState::languageIndex = DEFAULT_CONFIG.languageIndex;
    AppState::tankVolume = DEFAULT_CONFIG.tankVolume;
    AppState::timeOffset = DEFAULT_CONFIG.timeOffset;
    AppState::lowThreshold = DEFAULT_CONFIG.lowThreshold;
    AppState::highThreshold = DEFAULT_CONFIG.highThreshold;

    for (uint8_t i = 0; i < PUMP_COUNT; i++) {
      AppState::pumps[i].setAmount(DEFAULT_CONFIG.pumpAmounts[i]);
      AppState::pumps[i].setDuration(DEFAULT_CONFIG.pumpDurations[i]);
    }
  }
}

void saveAppStateToConfiguration() {
  Serial.println("[STORAGE] Saving AppState to configuration");

  Configuration config;

  config.languageIndex = AppState::languageIndex;
  config.tankVolume = AppState::tankVolume;
  config.timeOffset = AppState::timeOffset;
  config.lowThreshold = AppState::lowThreshold;
  config.highThreshold = AppState::highThreshold;

  for (uint8_t i = 0; i < PUMP_COUNT; i++) {
    config.pumpAmounts[i] = AppState::pumps[i].getAmount();
    config.pumpDurations[i] = AppState::pumps[i].getDuration();
  }

  saveConfiguration(config);
  Serial.println("[STORAGE] AppState saved to configuration");
}

void factoryReset() {
  Serial.println("[STORAGE] ==== FACTORY RESET =====");

  // Create a configuration with magic values
  Configuration resetConfig;

  resetConfig.languageIndex = UNSET_U8;
  resetConfig.tankVolume = UNSET_U32;
  resetConfig.timeOffset = UNSET_I64;
  resetConfig.lowThreshold = UNSET_U16;
  resetConfig.highThreshold = UNSET_U16;

  for (uint8_t i = 0; i < PUMP_COUNT; i++) {
    resetConfig.pumpAmounts[i] = UNSET_U16;
    resetConfig.pumpDurations[i] = UNSET_U64;
  }

  saveConfiguration(resetConfig);

  Serial.println("[STORAGE] Factory reset completed - all values set to unset state");
  Serial.println("[STORAGE] ====================================");
}