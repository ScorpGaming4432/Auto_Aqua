/**
 * ============================================================================
 * WATER.CPP - Water System Management
 * ============================================================================
 * 
 * Implements water pump control and water level sensing operations.
 * Provides functions to activate pumps and read sensor data via I2C.
*/

#include "pumps.h"
#include "sensor.h"
#include <Wire.h>
#include <Arduino.h>

// Water management state
namespace WaterManager {
  // Water level thresholds (can be configured)
  int16_t lowThreshold = 100;   // Below this, inlet pump activates
  int16_t highThreshold = 900;  // Above this, outlet pump activates
  
  // Pump modes (true = auto, false = manual)
  bool inletAutoMode = true;
  bool outletAutoMode = true;
  
  // Manual override states
  bool inletManualState = false;
  bool outletManualState = false;
  
  // Last water level reading
  uint16_t lastWaterLevel = 0;
  unsigned long lastCheckTime = 0;
  const unsigned long CHECK_INTERVAL = 5000; // Check every 5 seconds
  
  // Pump pin assignments (water pumps)
  const uint8_t INLET_PUMP_PIN = 6;  // Example pin for inlet pump
  const uint8_t OUTLET_PUMP_PIN = 7; // Example pin for outlet pump
}

/**
 * Initialize water management system
 * Sets up pump pins and initial state
 */
void initWaterManagement() {
  pinMode(WaterManager::INLET_PUMP_PIN, OUTPUT);
  pinMode(WaterManager::OUTLET_PUMP_PIN, OUTPUT);
  
  // Ensure pumps start in OFF state
  digitalWrite(WaterManager::INLET_PUMP_PIN, LOW);
  digitalWrite(WaterManager::OUTLET_PUMP_PIN, LOW);
  
  Serial.println("[WATER] Water management initialized");
  Serial.print("[WATER] Low threshold: ");
  Serial.println(WaterManager::lowThreshold);
  Serial.print("[WATER] High threshold: ");
  Serial.println(WaterManager::highThreshold);
}

/**
 * Check water level and control pumps automatically
 * Should be called periodically from main loop
 */
void checkWaterLevel() {
  unsigned long currentTime = millis();
  
  // Only check at specified intervals
  if (currentTime - WaterManager::lastCheckTime < WaterManager::CHECK_INTERVAL) {
    return;
  }
  
  WaterManager::lastCheckTime = currentTime;
  WaterManager::lastWaterLevel = read_water_sensor();
  
  Serial.print("[WATER] Level: ");
  Serial.print(WaterManager::lastWaterLevel);
  Serial.print(" (Low: ");
  Serial.print(WaterManager::lowThreshold);
  Serial.print(", High: ");
  Serial.print(WaterManager::highThreshold);
  Serial.println(")");
  
  // Check inlet pump (add water when level is low)
  if (WaterManager::inletAutoMode && !WaterManager::inletManualState) {
    if (WaterManager::lastWaterLevel < WaterManager::lowThreshold) {
      digitalWrite(WaterManager::INLET_PUMP_PIN, HIGH);
      Serial.println("[WATER] Inlet pump ON (low level)");
    } else {
      digitalWrite(WaterManager::INLET_PUMP_PIN, LOW);
      Serial.println("[WATER] Inlet pump OFF (level OK)");
    }
  }
  
  // Check outlet pump (remove water when level is high)
  if (WaterManager::outletAutoMode && !WaterManager::outletManualState) {
    if (WaterManager::lastWaterLevel > WaterManager::highThreshold) {
      digitalWrite(WaterManager::OUTLET_PUMP_PIN, HIGH);
      Serial.println("[WATER] Outlet pump ON (high level)");
    } else {
      digitalWrite(WaterManager::OUTLET_PUMP_PIN, LOW);
      Serial.println("[WATER] Outlet pump OFF (level OK)");
    }
  }
}

/**
 * Toggle inlet pump mode between auto and manual
 */
void toggleInletPumpMode() {
  WaterManager::inletAutoMode = !WaterManager::inletAutoMode;
  WaterManager::inletManualState = false; // Reset manual state when switching modes
  
  Serial.print("[WATER] Inlet pump mode: ");
  Serial.println(WaterManager::inletAutoMode ? "AUTO" : "MANUAL");
  
  // Turn off pump if switching to manual mode
  if (!WaterManager::inletAutoMode) {
    digitalWrite(WaterManager::INLET_PUMP_PIN, LOW);
  }
}

/**
 * Toggle outlet pump mode between auto and manual
 */
void toggleOutletPumpMode() {
  WaterManager::outletAutoMode = !WaterManager::outletAutoMode;
  WaterManager::outletManualState = false; // Reset manual state when switching modes
  
  Serial.print("[WATER] Outlet pump mode: ");
  Serial.println(WaterManager::outletAutoMode ? "AUTO" : "MANUAL");
  
  // Turn off pump if switching to manual mode
  if (!WaterManager::outletAutoMode) {
    digitalWrite(WaterManager::OUTLET_PUMP_PIN, LOW);
  }
}

/**
 * Manual control of inlet pump
 * @param turnOn true to turn on, false to turn off
 */
void setInletPumpManual(bool turnOn) {
  if (!WaterManager::inletAutoMode) {
    WaterManager::inletManualState = turnOn;
    digitalWrite(WaterManager::INLET_PUMP_PIN, turnOn ? HIGH : LOW);
    Serial.print("[WATER] Inlet pump manual: ");
    Serial.println(turnOn ? "ON" : "OFF");
  }
}

/**
 * Manual control of outlet pump
 * @param turnOn true to turn on, false to turn off
 */
void setOutletPumpManual(bool turnOn) {
  if (!WaterManager::outletAutoMode) {
    WaterManager::outletManualState = turnOn;
    digitalWrite(WaterManager::OUTLET_PUMP_PIN, turnOn ? HIGH : LOW);
    Serial.print("[WATER] Outlet pump manual: ");
    Serial.println(turnOn ? "ON" : "OFF");
  }
}

/**
 * Get current water level
 * @return Last water level reading
 */
uint16_t getCurrentWaterLevel() {
  return WaterManager::lastWaterLevel;
}

/**
 * Get inlet pump mode
 * @return true if auto mode, false if manual mode
 */
bool getInletPumpMode() {
  return WaterManager::inletAutoMode;
}

/**
 * Get outlet pump mode
 * @return true if auto mode, false if manual mode
 */
bool getOutletPumpMode() {
  return WaterManager::outletAutoMode;
}

/**
 * Set water level thresholds
 * @param low Low threshold for inlet pump activation
 * @param high High threshold for outlet pump activation
 */
void setWaterThresholds(int16_t low, int16_t high) {
  WaterManager::lowThreshold = low;
  WaterManager::highThreshold = high;
  Serial.print("[WATER] Thresholds set - Low: ");
  Serial.print(low);
  Serial.print(", High: ");
  Serial.println(high);
}

int16_t getLowThreshold() {
  return WaterManager::lowThreshold;
}

int16_t getHighThreshold() {
  return WaterManager::highThreshold;
}

/**
 * Activate a pump for a specified duration
 * Controls pump relay via digital pin to dispense precise amounts of liquid.
 * @param pump_pin Digital pin connected to pump relay (HIGH=on, LOW=off)
 * @param duration_ms Duration to keep pump running in milliseconds
 */
 void pump_work(uint8_t pump_pin, uint16_t duration_ms) {
  // Activate pump by setting pin HIGH
  digitalWrite(pump_pin, HIGH);
  // Wait for specified duration
  delay(duration_ms);
  // Deactivate pump by setting pin LOW
  digitalWrite(pump_pin, LOW);
}

/**
 * Read data from I2C water sensor device
 * Communicates with sensor at specified address and reads requested bytes.
 * @param addr I2C device address
 * @param length Number of bytes to read from device
 * @return Sum of all bytes read from the sensor
 */
 int readI2CData(byte addr, int length) {
  // Request specified number of bytes from I2C device
  Wire.requestFrom(addr, length);
  int sum = 0;
  // Sum all received bytes
  while (Wire.available()) {
    sum += Wire.read();
  }
  return sum;
}

/**
 * Read water level from dual-level sensors
 * Compares high and low level sensors to determine water level.
 * Outputs debugging information to serial console.
 * @return Difference between high and low sensor readings
 */
 uint16_t read_water_sensor() {
  // Read from upper sensor (high level detection)
  int highVal = readI2CData(ADDR_HIGH, 12);  // 12 bytes from upper section
  // Read from lower sensor (low level detection)
  int lowVal = readI2CData(ADDR_LOW, 8);     // 8 bytes from lower section

  // Debug output
  Serial.print("HighLevel: ");
  Serial.println(highVal);
  Serial.print("LowLevel : ");
  Serial.println(lowVal);

  // Return the difference as the water level indicator
  return (highVal - lowVal);
}