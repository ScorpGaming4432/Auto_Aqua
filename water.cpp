/**
 * ============================================================================
 * WATER.CPP - Water Management Implementation
 * ============================================================================
 * 
 * Implements water level monitoring and automatic pump control for the
 * aquarium system. Reads water level sensors and manages inlet/outlet pumps
 * based on configurable thresholds.
 */

#include "water.h"
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <string.h>
#include "pumps.h"

// ============================================================================
// Configuration Constants
// ============================================================================

// Pump pin assignments (adjust for your hardware)
#define INLET_PUMP_PIN 38
#define OUTLET_PUMP_PIN 39

// EEPROM storage addresses for water settings
#define EEPROM_ADDR_LOW_THRESHOLD 52
#define EEPROM_ADDR_HIGH_THRESHOLD 54

// I2C sensor addresses
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77

// Water level sensing constants
#define NO_TOUCH        0xFE
#define THRESHOLD       100

// Hysteresis to prevent pump cycling (percentage)
#define HYSTERESIS_MARGIN 5

// ============================================================================
// Global State Variables
// ============================================================================

// Water level thresholds (percentage, 0-100)
static int16_t lowThreshold = 40;
static int16_t highThreshold = 80;

// Last water check time (for periodic checks)
static unsigned long lastWaterCheck = 0;
static const unsigned long WATER_CHECK_INTERVAL = 5000;  // 5 seconds

// Pump runtime tracking
static uint32_t inletPumpTotalRuntime = 0;
static uint32_t outletPumpTotalRuntime = 0;
static unsigned long pumpStartTime = 0;
static bool pumpActive = false;
static uint8_t activePumpPin = 0;

// Error tracking
static WaterError currentError = WATER_ERROR_NONE;

// Hysteresis state
static bool inletPumpWasActive = false;
static bool outletPumpWasActive = false;

// ============================================================================
// WaterSensor class implementation
// ============================================================================

/**
 * Global WaterSensor instance
 * This is the main sensor object used throughout the module
 */
static WaterSensor waterSensor;

WaterSensor::WaterSensor() {
  memset(high_data, 0, sizeof(high_data));
  memset(low_data, 0, sizeof(low_data));
  lastError = WATER_ERROR_NONE;
  lastSuccessfulRead = 0;
  sensorConnected = false;
}

WaterError WaterSensor::readSensorData() {
  unsigned long startTime = millis();
  
  // Read low sensor with timeout
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  
  // Wait for data with timeout
  while (8 != Wire.available()) {
    if (millis() - startTime > SENSOR_READ_TIMEOUT_MS) {
      lastError = WATER_ERROR_SENSOR_TIMEOUT;
      sensorConnected = false;
      return lastError;
    }
  }
  
  for (int i = 0; i < 8; i++) {
    low_data[i] = Wire.read();
  }

  // Read high sensor with timeout
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  
  // Wait for data with timeout
  while (12 != Wire.available()) {
    if (millis() - startTime > SENSOR_READ_TIMEOUT_MS) {
      lastError = WATER_ERROR_SENSOR_TIMEOUT;
      sensorConnected = false;
      return lastError;
    }
  }
  
  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
  
  delay(10);
  
  // Validate sensor data
  bool validData = false;
  for (int i = 0; i < 8; i++) {
    if (low_data[i] != 0) validData = true;
  }
  for (int i = 0; i < 12; i++) {
    if (high_data[i] != 0) validData = true;
  }
  
  if (!validData) {
    lastError = WATER_ERROR_SENSOR_INVALID_DATA;
    sensorConnected = false;
    return lastError;
  }
  
  lastError = WATER_ERROR_NONE;
  lastSuccessfulRead = millis();
  sensorConnected = true;
  return lastError;
}

uint8_t WaterSensor::getTouchedSections() {
  readSensorData();

  uint32_t touch_val = 0;
  uint8_t trig_section = 0;

  for (int i = 0; i < 8; i++) {
    if (low_data[i] > THRESHOLD) {
      touch_val |= 1 << i;
    }
  }

  for (int i = 0; i < 12; i++) {
    if (high_data[i] > THRESHOLD) {
      touch_val |= (uint32_t)1 << (8 + i);
    }
  }

  while (touch_val & 0x01) {
    trig_section++;
    touch_val >>= 1;
  }

  return trig_section;
}

/**
 * Calculate water level based on touched sections
 * @return uint8_t Returns percentage (0-100)
 */
uint8_t WaterSensor::calculateWaterLevel() {
  uint8_t sections = getTouchedSections();
  return (sections * 5);
}

uint32_t WaterSensor::readWaterLevelRaw() {
  readSensorData();

  uint32_t touch_val = 0;
  for (int i = 0; i < 8; i++) {
    if (low_data[i] > THRESHOLD) {
      touch_val |= 1 << i;
    }
  }
  for (int i = 0; i < 12; i++) {
    if (high_data[i] > THRESHOLD) {
      touch_val |= (uint32_t)1 << (8 + i);
    }
  }
  return touch_val;
}

void WaterSensor::getCurrentWaterLevel(uint8_t *highBuf, uint8_t *lowBuf) {
  readSensorData();
  if (highBuf != NULL) memcpy(highBuf, high_data, 12);
  if (lowBuf != NULL) memcpy(lowBuf, low_data, 8);
}

WaterError WaterSensor::getLastError() const {
  return lastError;
}

bool WaterSensor::calibrateSensor(uint8_t sensorType, uint8_t *referenceData) {
  // Perform sensor calibration with multiple readings
  if (referenceData == NULL) return false;
  
  uint8_t calibrationReadings = 5;
  uint8_t sumLow[8] = {0};
  uint8_t sumHigh[12] = {0};
  
  // Take multiple readings for averaging
  for (int i = 0; i < calibrationReadings; i++) {
    if (readSensorData() != WATER_ERROR_NONE) {
      return false;
    }
    
    for (int j = 0; j < 8; j++) {
      sumLow[j] += low_data[j];
    }
    for (int j = 0; j < 12; j++) {
      sumHigh[j] += high_data[j];
    }
    delay(100); // Wait between readings
  }
  
  // Average the readings
  if (sensorType == 0) {
    for (int i = 0; i < 8; i++) {
      referenceData[i] = sumLow[i] / calibrationReadings;
    }
    return true;
  } else if (sensorType == 1) {
    for (int i = 0; i < 12; i++) {
      referenceData[i] = sumHigh[i] / calibrationReadings;
    }
    return true;
  }
  
  return false;
}

bool WaterSensor::isSensorConnected() const {
  return sensorConnected && (millis() - lastSuccessfulRead < SENSOR_READ_TIMEOUT_MS * 3);
}

// ============================================================================
// Public Water Management Functions
// ============================================================================

void initWaterManagement() {
  // Initialize pump control pins
  pinMode(INLET_PUMP_PIN, OUTPUT);
  pinMode(OUTLET_PUMP_PIN, OUTPUT);
  digitalWrite(INLET_PUMP_PIN, LOW);
  digitalWrite(OUTLET_PUMP_PIN, LOW);
  
  // Initialize pump configurations
  initPumpModes();

  // Load water level thresholds from EEPROM
  uint16_t lowStored = (EEPROM.read(EEPROM_ADDR_LOW_THRESHOLD) << 8) | 
                       EEPROM.read(EEPROM_ADDR_LOW_THRESHOLD + 1);
  uint16_t highStored = (EEPROM.read(EEPROM_ADDR_HIGH_THRESHOLD) << 8) | 
                        EEPROM.read(EEPROM_ADDR_HIGH_THRESHOLD + 1);
  
  // Use stored values if valid, otherwise keep defaults
  if (lowStored != 0xFFFF && lowStored >= 0 && lowStored <= 100) {
    lowThreshold = lowStored;
  }
  if (highStored != 0xFFFF && highStored >= 0 && highStored <= 100) {
    highThreshold = highStored;
  }
  
  // Validate threshold relationship
  if (lowThreshold >= highThreshold) {
    // Reset to safe defaults
    lowThreshold = 30;
    highThreshold = 70;
    Serial.println("[WATER] Invalid thresholds - reset to defaults");
  }
  
  // Initialize hysteresis state
  inletPumpWasActive = false;
  outletPumpWasActive = false;
  
  Serial.println("[WATER] Water management initialized");
  Serial.println("[WATER] Inlet pump: AUTO");
  Serial.println("[WATER] Outlet pump: AUTO");
  Serial.print("[WATER] Thresholds: ");
  Serial.print(lowThreshold);
  Serial.print("% - ");
  Serial.print(highThreshold);
  Serial.println("%");
  Serial.print("[WATER] Hysteresis margin: ");
  Serial.print(HYSTERESIS_MARGIN);
  Serial.println("%");
}

void checkWaterLevel() {
  unsigned long now = millis();
  
  // Throttle checks to avoid excessive I2C traffic
  if (now - lastWaterCheck < WATER_CHECK_INTERVAL) {
    return;
  }
  lastWaterCheck = now;
  
  // Check sensor health first
  if (!checkSensorHealth()) {
    emergencyStopLetPumps();
    return;
  }
  
  uint8_t currentLevel = waterSensor.calculateWaterLevel();
  Serial.print("[WATER] Level: ");
  Serial.print(currentLevel);
  Serial.println("%");
  
  // Inlet pump control with hysteresis (always automatic)
  if (currentLevel < lowThreshold - HYSTERESIS_MARGIN) {
    if (!inletPumpWasActive) {
      Serial.println("[WATER] Inlet pump ON (level too low)");
      runPumpSafely(INLET_PUMP_PIN, calculatePumpDuration(currentLevel, lowThreshold));
      inletPumpWasActive = true;
    }
  } else if (currentLevel > lowThreshold + HYSTERESIS_MARGIN) {
    inletPumpWasActive = false;
  }
  
  // Outlet pump control with hysteresis (always automatic)
  if (currentLevel > highThreshold + HYSTERESIS_MARGIN) {
    if (!outletPumpWasActive) {
      Serial.println("[WATER] Outlet pump ON (level too high)");
      runPumpSafely(OUTLET_PUMP_PIN, calculatePumpDuration(currentLevel, highThreshold));
      outletPumpWasActive = true;
    }
  } else if (currentLevel < highThreshold - HYSTERESIS_MARGIN) {
    outletPumpWasActive = false;
  }
}

// Pump mode functions moved to pumps module

// Helper function to safely run pumps with timeout protection
void runPumpSafely(uint8_t pumpPin, uint16_t duration) {
  if (pumpActive) {
    Serial.println("[WATER] Pump already active - skipping");
    return;
  }
  
  if (duration > MAX_PUMP_RUN_TIME_MS) {
    Serial.println("[WATER] Pump duration too long - limiting");
    duration = MAX_PUMP_RUN_TIME_MS;
  }
  
  pumpActive = true;
  activePumpPin = pumpPin;
  pumpStartTime = millis();
  
  digitalWrite(pumpPin, HIGH);
  
  // Track runtime
  if (pumpPin == INLET_PUMP_PIN) {
    inletPumpTotalRuntime += duration;
  } else if (pumpPin == OUTLET_PUMP_PIN) {
    outletPumpTotalRuntime += duration;
  }
  
  // Wait with timeout protection
  unsigned long startWait = millis();
  while (millis() - startWait < duration) {
    if (millis() - pumpStartTime > MAX_PUMP_RUN_TIME_MS) {
      Serial.println("[WATER] Pump timeout - stopping");
      break;
    }
    delay(100); // Check every 100ms
  }
  
  digitalWrite(pumpPin, LOW);
  pumpActive = false;
}

/**
 * Calculate pump duration based on water level deviation from target
 * @param currentLevel Current water level percentage (0-100)
 * @param target Target percentage (low or high)
 * @return Duration in milliseconds to run the pump
 */
uint16_t calculatePumpDuration(uint8_t currentLevel, uint8_t target) {
  // Calculate deviation from target
  uint8_t deviation = abs(currentLevel - target);
  
  // Base duration + proportional adjustment based on deviation
  // Minimum 1000ms, maximum 10000ms (10 seconds)
  uint16_t duration = 1000 + (deviation * 100);
  
  // Cap at maximum runtime
  if (duration > MAX_PUMP_RUN_TIME_MS) {
    duration = MAX_PUMP_RUN_TIME_MS;
  }
  
  Serial.print("[WATER] Pump duration calculated: ");
  Serial.print(duration);
  Serial.println("ms");
  
  return duration;
}

int16_t getLowThreshold() {
  return lowThreshold;
}

int16_t getHighThreshold() {
  return highThreshold;
}

void setLowThreshold(int16_t threshold) {
  if (threshold >= 0 && threshold < highThreshold) {
    lowThreshold = threshold;
    EEPROM.write(EEPROM_ADDR_LOW_THRESHOLD, (threshold >> 8) & 0xFF);
    EEPROM.write(EEPROM_ADDR_LOW_THRESHOLD + 1, threshold & 0xFF);
    Serial.print("[WATER] Low threshold set to: ");
    Serial.print(threshold);
    Serial.println("%");
  }
}

void setHighThreshold(int16_t threshold) {
  if (threshold > lowThreshold && threshold <= 100) {
    highThreshold = threshold;
    EEPROM.write(EEPROM_ADDR_HIGH_THRESHOLD, (threshold >> 8) & 0xFF);
    EEPROM.write(EEPROM_ADDR_HIGH_THRESHOLD + 1, threshold & 0xFF);
    Serial.print("[WATER] High threshold set to: ");
    Serial.print(threshold);
    Serial.println("%");
  }
}

void setWaterThresholds(int16_t low, int16_t high) {
  if (low >= 0 && high <= 100 && low < high) {
    lowThreshold = low;
    highThreshold = high;
    EEPROM.write(EEPROM_ADDR_LOW_THRESHOLD, (low >> 8) & 0xFF);
    EEPROM.write(EEPROM_ADDR_LOW_THRESHOLD + 1, low & 0xFF);
    EEPROM.write(EEPROM_ADDR_HIGH_THRESHOLD, (high >> 8) & 0xFF);
    EEPROM.write(EEPROM_ADDR_HIGH_THRESHOLD + 1, high & 0xFF);
    Serial.print("[WATER] Thresholds set - Low: ");
    Serial.print(low);
    Serial.print("% High: ");
    Serial.print(high);
    Serial.println("%");
  } else {
    Serial.println("[WATER] Invalid thresholds - not saved");
  }
}

void getCurrentWaterLevel(uint8_t *highBuf, uint8_t *lowBuf) {
  // Use static buffers to avoid stack allocation
  static uint8_t tmpHigh[12];
  static uint8_t tmpLow[8];
  uint8_t *h = highBuf ? highBuf : tmpHigh;
  uint8_t *l = lowBuf ? lowBuf : tmpLow;

  waterSensor.getCurrentWaterLevel(h, l);

  // Print sensor data to serial for debugging (only if buffers provided)
  if (highBuf == NULL || lowBuf == NULL) {
    Serial.print("[WATER] High sensor: ");
    for (int i = 0; i < 12; i++) {
      Serial.print(h[i]);
      Serial.print(" ");
    }
    Serial.println();

    Serial.print("[WATER] Low sensor: ");
    for (int i = 0; i < 8; i++) {
      Serial.print(l[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
}

void read_water_sensor(uint8_t *highBuf, uint8_t *lowBuf) {
  if (highBuf == NULL && lowBuf == NULL) {
    // nothing to do
    return;
  }
  waterSensor.getCurrentWaterLevel(highBuf, lowBuf);
}

WaterError getWaterError() {
  return currentError;
}

void clearWaterError() {
  currentError = WATER_ERROR_NONE;
}

bool checkSensorHealth() {
  WaterError error = waterSensor.readSensorData();
  if (error != WATER_ERROR_NONE) {
    currentError = error;
    Serial.print("[WATER] Sensor error: ");
    Serial.println(error);
    return false;
  }
  return waterSensor.isSensorConnected();
}

void emergencyStopLetPumps() {
  digitalWrite(INLET_PUMP_PIN, LOW);
  digitalWrite(OUTLET_PUMP_PIN, LOW);
  pumpActive = false;
  Serial.println("[WATER] Emergency stop - all pumps deactivated");
}

void getPumpStatistics(uint32_t *inletRuntime, uint32_t *outletRuntime) {
  if (inletRuntime != NULL) *inletRuntime = inletPumpTotalRuntime;
  if (outletRuntime != NULL) *outletRuntime = outletPumpTotalRuntime;
}

void resetPumpStatistics() {
  inletPumpTotalRuntime = 0;
  outletPumpTotalRuntime = 0;
  Serial.println("[WATER] Pump statistics reset");
}

// (Duplicate calculatePumpDuration removed — single implementation retained earlier)
