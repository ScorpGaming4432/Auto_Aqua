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
#include "waterlevelsensor.h"
#include <Arduino.h>
#include <Wire.h>
#include <EEPROM.h>
#include <string.h>

// ============================================================================
// Configuration Constants
// ============================================================================

// Pump pin assignments (adjust for your hardware)
#define INLET_PUMP_PIN 38
#define OUTLET_PUMP_PIN 39

// EEPROM storage addresses for water settings
#define EEPROM_ADDR_INLET_MODE 50
#define EEPROM_ADDR_OUTLET_MODE 51
#define EEPROM_ADDR_LOW_THRESHOLD 52
#define EEPROM_ADDR_HIGH_THRESHOLD 54

// I2C sensor addresses (from waterlevelsensor.cpp)
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR 0x77

// ============================================================================
// Global State Variables
// ============================================================================

// Pump mode: AUTO (1) or MANUAL (0)
static bool inletPumpAuto = true;
static bool outletPumpAuto = true;

// Water level thresholds (percentage, 0-100)
static int16_t lowThreshold = 30;
static int16_t highThreshold = 70;

// Raw sensor data buffers (persist for getCurrentWaterLevel)
static uint8_t g_high_data[12] = {0};
static uint8_t g_low_data[8] = {0};

// Last water check time (for periodic checks)
static unsigned long lastWaterCheck = 0;
static const unsigned long WATER_CHECK_INTERVAL = 5000;  // 5 seconds

// ============================================================================
// Internal Helper Functions
// ============================================================================

/**
 * Read raw touch sensor data from both I2C sensors
 * Stores data in global buffers
 */
static void readSensorData() {
  memset(g_low_data, 0, sizeof(g_low_data));
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  while (8 != Wire.available());
  for (int i = 0; i < 8; i++) {
    g_low_data[i] = Wire.read();
  }

  memset(g_high_data, 0, sizeof(g_high_data));
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  while (12 != Wire.available());
  for (int i = 0; i < 12; i++) {
    g_high_data[i] = Wire.read();
  }
  delay(10);
}

/**
 * Calculate current water level percentage from sensor data
 * @return Water level as percentage (0-100)
 */
static uint8_t calculateWaterLevel() {
  readSensorData();
  
  uint32_t touch_val = 0;
  uint8_t trig_section = 0;
  const int THRESHOLD = 100;
  
  // Combine sensor readings into single value
  for (int i = 0; i < 8; i++) {
    if (g_low_data[i] > THRESHOLD) {
      touch_val |= 1 << i;
    }
  }
  
  for (int i = 0; i < 12; i++) {
    if (g_high_data[i] > THRESHOLD) {
      touch_val |= (uint32_t)1 << (8 + i);
    }
  }
  
  // Count leading zeros to get section count
  while (touch_val & 0x01) {
    trig_section++;
    touch_val >>= 1;
  }
  
  // Each section represents approximately 5%
  return (trig_section * 5);
}

// ============================================================================
// Public Water Management Functions
// ============================================================================

void initWaterManagement() {
  // Configure pump pins as outputs
  pinMode(INLET_PUMP_PIN, OUTPUT);
  pinMode(OUTLET_PUMP_PIN, OUTPUT);
  digitalWrite(INLET_PUMP_PIN, LOW);
  digitalWrite(OUTLET_PUMP_PIN, LOW);
  
  // Load settings from EEPROM
  inletPumpAuto = EEPROM.read(EEPROM_ADDR_INLET_MODE) ? true : false;
  outletPumpAuto = EEPROM.read(EEPROM_ADDR_OUTLET_MODE) ? true : false;
  
  // Load thresholds (default to 30% and 70% if not set)
  uint16_t lowStored = (EEPROM.read(EEPROM_ADDR_LOW_THRESHOLD) << 8) | 
                       EEPROM.read(EEPROM_ADDR_LOW_THRESHOLD + 1);
  uint16_t highStored = (EEPROM.read(EEPROM_ADDR_HIGH_THRESHOLD) << 8) | 
                        EEPROM.read(EEPROM_ADDR_HIGH_THRESHOLD + 1);
  
  if (lowStored != 0xFFFF) lowThreshold = lowStored;
  if (highStored != 0xFFFF) highThreshold = highStored;
  
  Serial.println("[WATER] Water management initialized");
  Serial.print("[WATER] Inlet: ");
  Serial.println(inletPumpAuto ? "AUTO" : "MANUAL");
  Serial.print("[WATER] Outlet: ");
  Serial.println(outletPumpAuto ? "AUTO" : "MANUAL");
  Serial.print("[WATER] Thresholds: ");
  Serial.print(lowThreshold);
  Serial.print("% - ");
  Serial.print(highThreshold);
  Serial.println("%");
}

void checkWaterLevel() {
  unsigned long now = millis();
  
  // Throttle checks to avoid excessive I2C traffic
  if (now - lastWaterCheck < WATER_CHECK_INTERVAL) {
    return;
  }
  lastWaterCheck = now;
  
  uint8_t currentLevel = calculateWaterLevel();
  Serial.print("[WATER] Level: ");
  Serial.print(currentLevel);
  Serial.println("%");
  
  // Inlet pump control: activates when level drops below low threshold
  if (inletPumpAuto) {
    if (currentLevel < lowThreshold) {
      Serial.println("[WATER] Inlet pump ON (level too low)");
      digitalWrite(INLET_PUMP_PIN, HIGH);
      delay(1000);  // Run for 1 second
      digitalWrite(INLET_PUMP_PIN, LOW);
    }
  }
  
  // Outlet pump control: activates when level rises above high threshold
  if (outletPumpAuto) {
    if (currentLevel > highThreshold) {
      Serial.println("[WATER] Outlet pump ON (level too high)");
      digitalWrite(OUTLET_PUMP_PIN, HIGH);
      delay(1000);  // Run for 1 second
      digitalWrite(OUTLET_PUMP_PIN, LOW);
    }
  }
}

void toggleInletPumpMode() {
  inletPumpAuto = !inletPumpAuto;
  EEPROM.write(EEPROM_ADDR_INLET_MODE, inletPumpAuto ? 1 : 0);
  Serial.print("[WATER] Inlet pump mode: ");
  Serial.println(inletPumpAuto ? "AUTO" : "MANUAL");
}

void toggleOutletPumpMode() {
  outletPumpAuto = !outletPumpAuto;
  EEPROM.write(EEPROM_ADDR_OUTLET_MODE, outletPumpAuto ? 1 : 0);
  Serial.print("[WATER] Outlet pump mode: ");
  Serial.println(outletPumpAuto ? "AUTO" : "MANUAL");
}

bool getInletPumpMode() {
  return inletPumpAuto;
}

bool getOutletPumpMode() {
  return outletPumpAuto;
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

void getCurrentWaterLevel(uint8_t *highBuf, uint8_t *lowBuf) {
  readSensorData();
  
  if (highBuf != NULL) {
    memcpy(highBuf, g_high_data, 12);
  }
  if (lowBuf != NULL) {
    memcpy(lowBuf, g_low_data, 8);
  }
  
  // Print sensor data to serial for debugging
  Serial.print("[WATER] High sensor: ");
  for (int i = 0; i < 12; i++) {
    Serial.print(g_high_data[i]);
    Serial.print(" ");
  }
  Serial.println();
  
  Serial.print("[WATER] Low sensor: ");
  for (int i = 0; i < 8; i++) {
    Serial.print(g_low_data[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void read_water_sensor(uint8_t *highBuf, uint8_t *lowBuf) {
  readSensorData();
  
  if (highBuf != NULL) {
    memcpy(highBuf, g_high_data, 12);
  }
  if (lowBuf != NULL) {
    memcpy(lowBuf, g_low_data, 8);
  }
}
