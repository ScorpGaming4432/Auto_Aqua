/**
 * ============================================================================
 * STORAGE.CPP - Data Storage Implementation
 * ============================================================================
 * 
 * Implements persistent storage of configuration data using Arduino EEPROM.
 * Handles reading/writing settings like tank volume, pump amounts, etc.
 * 
 * Returns maximum values (0xFF repeated) if data not set, allowing 0 to be a valid user value.
 */

#include "storage.h"
#include "language.h"
#include <Arduino.h>
#include <EEPROM.h>
#include <stdint.h>
// Helper function to write 32-bit value to EEPROM
static void writeEEPROM32(uint16_t address, uint32_t value) {
  EEPROM.write(address, (uint8_t)(value & 0xFF));
  EEPROM.write(address + 1, (uint8_t)((value >> 8) & 0xFF));
  EEPROM.write(address + 2, (uint8_t)((value >> 16) & 0xFF));
  EEPROM.write(address + 3, (uint8_t)((value >> 24) & 0xFF));
}

// Helper function to read 32-bit value from EEPROM
static uint32_t readEEPROM32(uint16_t address) {
  uint32_t value = EEPROM.read(address);
  value |= ((uint32_t)EEPROM.read(address + 1) << 8);
  value |= ((uint32_t)EEPROM.read(address + 2) << 16);
  value |= ((uint32_t)EEPROM.read(address + 3) << 24);
  return value;
}

// Helper function to write 64-bit value to EEPROM
static void writeEEPROM64(uint16_t address, uint64_t value) {
  EEPROM.write(address, (uint8_t)(value & 0xFF));
  EEPROM.write(address + 1, (uint8_t)((value >> 8) & 0xFF));
  EEPROM.write(address + 2, (uint8_t)((value >> 16) & 0xFF));
  EEPROM.write(address + 3, (uint8_t)((value >> 24) & 0xFF));
  EEPROM.write(address + 4, (uint8_t)((value >> 32) & 0xFF));
  EEPROM.write(address + 5, (uint8_t)((value >> 40) & 0xFF));
  EEPROM.write(address + 6, (uint8_t)((value >> 48) & 0xFF));
  EEPROM.write(address + 7, (uint8_t)((value >> 56) & 0xFF));
}

// Helper function to read 64-bit value from EEPROM
static uint64_t readEEPROM64(uint16_t address) {
  uint64_t value = EEPROM.read(address);
  value |= ((uint64_t)EEPROM.read(address + 1) << 8);
  value |= ((uint64_t)EEPROM.read(address + 2) << 16);
  value |= ((uint64_t)EEPROM.read(address + 3) << 24);
  value |= ((uint64_t)EEPROM.read(address + 4) << 32);
  value |= ((uint64_t)EEPROM.read(address + 5) << 40);
  value |= ((uint64_t)EEPROM.read(address + 6) << 48);
  value |= ((uint64_t)EEPROM.read(address + 7) << 56);
  return value;
}

void saveLanguageIndex(uint8_t languageIndex) {
  EEPROM.write(EEPROM_ADDR_LANGUAGE_INDEX, languageIndex);
  Serial.print("[STORAGE] Saved language index: ");
  Serial.println(languageIndex);
}

uint8_t loadLanguageIndex() {
  uint8_t value = EEPROM.read(EEPROM_ADDR_LANGUAGE_INDEX);
  Serial.print("[STORAGE] Loaded language index: ");
  Serial.println(value);
  // If all bits are 1, consider it not set (return 0)
  if (value == 0xFF) {
    return 0;
  }
  return value;
}

void saveTankVolume(uint32_t tankVolume) {
  writeEEPROM32(EEPROM_ADDR_TANK_VOLUME, tankVolume);
  Serial.print("[STORAGE] Saved tank volume: ");
  Serial.println(tankVolume);
}

uint32_t loadTankVolume() {
  uint32_t value = readEEPROM32(EEPROM_ADDR_TANK_VOLUME);
  Serial.print("[STORAGE] Loaded tank volume: ");
  Serial.println(value);
  return value;  // 0xFFFFFFFF means not set, but 0 is valid
}

void saveTimeOffset(int64_t timeOffset) {
  writeEEPROM64(EEPROM_ADDR_TIME_OFFSET, (uint64_t)timeOffset);
  Serial.print("[STORAGE] Saved time offset: ");
  Serial.println((long)timeOffset);
}

int64_t loadTimeOffset() {
  uint64_t value = readEEPROM64(EEPROM_ADDR_TIME_OFFSET);
  Serial.print("[STORAGE] Loaded time offset: ");
  Serial.println((long)value);
  return (int64_t)value;
}

void savePumpAmount(uint8_t pumpIndex, uint16_t amount) {
  uint16_t address = EEPROM_ADDR_PUMP_AMOUNTS + (pumpIndex * 2);
  EEPROM.write(address, (uint8_t)(amount & 0xFF));
  EEPROM.write(address + 1, (uint8_t)((amount >> 8) & 0xFF));
  Serial.print("[STORAGE] Saved pump[");
  Serial.print(pumpIndex);
  Serial.print("] amount: ");
  Serial.println(amount);
}

uint16_t loadPumpAmount(uint8_t pumpIndex) {
  uint16_t address = EEPROM_ADDR_PUMP_AMOUNTS + (pumpIndex * 2);
  uint16_t value = EEPROM.read(address);
  value |= ((uint16_t)EEPROM.read(address + 1) << 8);
  Serial.print("[STORAGE] Loaded pump[");
  Serial.print(pumpIndex);
  Serial.print("] amount: ");
  Serial.println(value);
  return value;  // 0xFFFF means not set, 0 means pump not used
}

void savePumpDuration(uint8_t pumpIndex, uint64_t duration) {
  uint16_t address = EEPROM_ADDR_PUMP_DURATIONS + (pumpIndex * 8);
  writeEEPROM64(address, duration);
  Serial.print("[STORAGE] Saved pump[");
  Serial.print(pumpIndex);
  Serial.print("] duration: ");
  Serial.println((unsigned long)duration);
}

uint64_t loadPumpDuration(uint8_t pumpIndex) {
  uint16_t address = EEPROM_ADDR_PUMP_DURATIONS + (pumpIndex * 8);
  uint64_t value = readEEPROM64(address);
  Serial.print("[STORAGE] Loaded pump[");
  Serial.print(pumpIndex);
  Serial.print("] duration: ");
  Serial.println((unsigned long)value);
  return value;  // 0xFFFFFFFFFFFFFFFF means not set, 0 means pump not used
}

void loadAllConfiguration() {
  Serial.println("[STORAGE] Loading all configuration from EEPROM");
  // This will be called from main sketch after AppState is available
}

void saveAllConfiguration() {
  Serial.println("[STORAGE] Saving all configuration to EEPROM");
  // This will be called from main sketch when values change
}

void factoryReset() {
  Serial.println("[STORAGE] ==== FACTORY RESET =====");
  
  // Reset language index to 0xFF
  EEPROM.write(EEPROM_ADDR_LANGUAGE_INDEX, 0xFF);
  Serial.println("[STORAGE] Reset LanguageIndex to 0xFF");
  
  // Reset tank volume to 0xFFFFFFFF
  writeEEPROM32(EEPROM_ADDR_TANK_VOLUME, 0xFFFFFFFF);
  
  // Reset time offset to 0xFFFFFFFFFFFFFFFF
  writeEEPROM64(EEPROM_ADDR_TIME_OFFSET, 0xFFFFFFFFFFFFFFFF);
  
  // Reset all pump amounts to 0xFFFF
  for (uint8_t i = 0; i < PUMP_COUNT; ++i) {  // Use PUMP_COUNT instead of hardcoded 5
    uint16_t address = EEPROM_ADDR_PUMP_AMOUNTS + (i * 2);
    EEPROM.write(address, 0xFF);
    EEPROM.write(address + 1, 0xFF);
    Serial.print("[STORAGE] Reset pump[");
    Serial.print(i);
    Serial.println("] amount to 0xFFFF");
  }
  
  // Reset all pump durations to 0xFFFFFFFFFFFFFFFF
  for (uint8_t i = 0; i < PUMP_COUNT; ++i) {  // Use PUMP_COUNT instead of hardcoded 5
    uint16_t address = EEPROM_ADDR_PUMP_DURATIONS + (i * 8);
    writeEEPROM64(address, 0xFFFFFFFFFFFFFFFF);
    Serial.print("[STORAGE] Reset pump[");
    Serial.print(i);
    Serial.println("] duration to 0xFFFFFFFFFFFFFFFF");
  }
  
  Serial.println("[STORAGE] Factory reset completed - all values set to unset state");
  Serial.println("[STORAGE] ====================================");
}