/**
 * ============================================================================
 * WATER_SENSOR.CPP - Water Sensor Implementation
 * ============================================================================
 */

#include "water.h"
#include "hardware.h"
#include "debug.h"
#include <Arduino.h>
#include <Wire.h>

// Static member variable definition
bool WaterSensor::initialized = false;

// Global instance of WaterSensor
WaterSensor waterSensor;

WaterSensor::WaterSensor() {
  if (initialized) return;
  initialized = true;
  memset(high_data, 0, sizeof(high_data));
  memset(low_data, 0, sizeof(low_data));
  lastError = WATER_ERROR_NONE;
  lastSuccessfulRead = 0;
  sensorConnected = false;
}

WaterError WaterSensor::readSensorData() {
  unsigned long startTime = millis();

  // Read low sensor
  memset(low_data, 0, sizeof(low_data));
  Wire.requestFrom(static_cast<uint8_t>(Hardware::WATER_SENSOR_LOW_ADDR), static_cast<uint8_t>(8));
  while (8 != Wire.available()) {
    if (millis() - startTime > Hardware::SENSOR_READ_TIMEOUT_MS) {
      lastError = WATER_ERROR_SENSOR_TIMEOUT;
      sensorConnected = false;
      return lastError;
    }
  }
  for (int i = 0; i < 8; i++) low_data[i] = Wire.read();

  // Read high sensor
  memset(high_data, 0, sizeof(high_data));
  Wire.requestFrom(static_cast<uint8_t>(Hardware::WATER_SENSOR_HIGH_ADDR), static_cast<uint8_t>(12));
  while (12 != Wire.available()) {
    if (millis() - startTime > Hardware::SENSOR_READ_TIMEOUT_MS) {
      lastError = WATER_ERROR_SENSOR_TIMEOUT;
      sensorConnected = false;
      return lastError;
    }
  }
  for (int i = 0; i < 12; i++) high_data[i] = Wire.read();

  delay(10); // Small delay for stability

  bool validData = false;
  for (int i = 0; i < 8; i++) if (low_data[i] != 0) validData = true;
  for (int i = 0; i < 12; i++) if (high_data[i] != 0) validData = true;

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
  for (int i = 0; i < 8; i++) if (low_data[i] > Hardware::TOUCH_THRESHOLD) touch_val |= 1 << i;
  for (int i = 0; i < 12; i++) if (high_data[i] > Hardware::TOUCH_THRESHOLD) touch_val |= static_cast<uint32_t>(1) << (8 + i);
  while (touch_val & 0x01) { trig_section++; touch_val >>= 1; }
  return trig_section;
}

uint8_t WaterSensor::calculateWaterLevel() {
  uint8_t sections = getTouchedSections();
  return (sections * 5);
}

uint32_t WaterSensor::readWaterLevelRaw() {
  readSensorData();
  uint32_t touch_val = 0;
  for (int i = 0; i < 8; i++) if (low_data[i] > Hardware::TOUCH_THRESHOLD) touch_val |= 1 << i;
  for (int i = 0; i < 12; i++) if (high_data[i] > Hardware::TOUCH_THRESHOLD) touch_val |= static_cast<uint32_t>(1) << (8 + i);
  return touch_val;
}

void WaterSensor::getCurrentWaterLevel(uint8_t *highBuf, uint8_t *lowBuf) {
  readSensorData();
  if (highBuf != NULL) memcpy(highBuf, high_data, 12);
  if (lowBuf != NULL) memcpy(lowBuf, low_data, 8);
}

WaterError WaterSensor::getLastError() const { return lastError; }

bool WaterSensor::calibrateSensor(uint8_t sensorType, uint8_t *referenceData) {
  if (referenceData == NULL) return false;
  uint8_t calibrationReadings = 5;
  uint32_t sumLow[8] = { 0 }, sumHigh[12] = { 0 };

  for (int i = 0; i < calibrationReadings; i++) {
    if (readSensorData() != WATER_ERROR_NONE) return false;
    for (int j = 0; j < 8; j++) sumLow[j] += low_data[j];
    for (int j = 0; j < 12; j++) sumHigh[j] += high_data[j];
    delay(100);
  }

  if (sensorType == 0) {
    for (int i = 0; i < 8; i++) referenceData[i] = sumLow[i] / calibrationReadings;
    return true;
  } else if (sensorType == 1) {
    for (int i = 0; i < 12; i++) referenceData[i] = sumHigh[i] / calibrationReadings;
    return true;
  }
  return false;
}

bool WaterSensor::isSensorConnected() const {
  return sensorConnected && (millis() - lastSuccessfulRead < Hardware::SENSOR_READ_TIMEOUT_MS * 3);
}
