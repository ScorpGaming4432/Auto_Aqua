/**
 * ============================================================================
 * STORAGE.H - Data Storage Interface (EEPROM/Flash)
 * ============================================================================
 * 
 * Defines storage and retrieval functions for persistent configuration data.
 * Stores settings like tank volume, pump amounts, and other user configurations.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>

// EEPROM addresses for different data
#define EEPROM_ADDR_LANGUAGE_INDEX 0
#define EEPROM_ADDR_TANK_VOLUME 1
#define EEPROM_ADDR_TIME_OFFSET 5
#define EEPROM_ADDR_PUMP_AMOUNTS 13
#define EEPROM_ADDR_PUMP_DURATIONS 23

/**
 * Save language index to EEPROM
 * @param languageIndex Language index to save
 */
void saveLanguageIndex(uint8_t languageIndex);

/**
 * Load language index from EEPROM
 * @return Language index (0 if not set)
 */
uint8_t loadLanguageIndex();

/**
 * Save tank volume to EEPROM
 * @param tankVolume Tank volume in liters/ml
 */
void saveTankVolume(uint32_t tankVolume);

/**
 * Load tank volume from EEPROM
 * @return Tank volume (0xFFFFFFFF if not set, 0 is valid)
 */
uint32_t loadTankVolume();

/**
 * Save time offset to EEPROM
 * @param timeOffset Time offset in seconds
 */
void saveTimeOffset(int64_t timeOffset);

/**
 * Load time offset from EEPROM
 * @return Time offset in seconds (0 if not set)
 */
int64_t loadTimeOffset();

/**
 * Save pump amount to EEPROM
 * @param pumpIndex Index of the pump (0 to PUMP_COUNT-1)
 * @param amount Amount to dispense (0 means pump not used)
 */
void savePumpAmount(uint8_t pumpIndex, uint16_t amount);

/**
 * Load pump amount from EEPROM
 * @param pumpIndex Index of the pump (0 to PUMP_COUNT-1)
 * @return Pump amount (0xFFFF if not set, 0 means pump not used)
 */
uint16_t loadPumpAmount(uint8_t pumpIndex);

/**
 * Save pump duration to EEPROM
 * @param pumpIndex Index of the pump (0 to PUMP_COUNT-1)
 * @param duration Duration in milliseconds
 */
void savePumpDuration(uint8_t pumpIndex, uint64_t duration);

/**
 * Load pump duration from EEPROM
 * @param pumpIndex Index of the pump (0 to PUMP_COUNT-1)
 * @return Pump duration in milliseconds (0xFFFFFFFFFFFFFFFF if not set, 0 means pump not used)
 */
uint64_t loadPumpDuration(uint8_t pumpIndex);

/**
 * Load all configuration from EEPROM
 * Used during setup to restore previous settings
 */
void loadAllConfiguration();

/**
 * Save all configuration to EEPROM
 * Used after any change to persist settings
 */
void saveAllConfiguration();

/**
 * Factory reset - sets all EEPROM values to unset state (0xFF repeated)
 */
void factoryReset();

#endif