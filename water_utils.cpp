/**
 * ============================================================================
 * WATER_UTILS.CPP - Water Utilities and Display
 * ============================================================================
 */

#include "water.h"
#include "appstate.h"
#include "storage.h"
#include "display.h"
#include <Arduino.h>

extern WaterSensor waterSensor;

uint8_t calculateWaterLevel() {
  return waterSensor.calculateWaterLevel();
}

uint16_t calculatePumpDuration(uint8_t currentLevel, uint8_t target) {
  uint8_t deviation = abs(currentLevel - target);
  uint16_t duration = 1000 + (deviation * 100);
  if (duration > Hardware::MAX_PUMP_RUN_TIME_MS) duration = Hardware::MAX_PUMP_RUN_TIME_MS;
  return duration;
}

void displayWaterLevelStatus(const WaterLevelResult &result) {
  lcd.clear();
  if (result.error != WATER_ERROR_NONE) {
    lcd.setCursor(0, 0);
    lcd.print("Water Sensor Err");
    lcd.setCursor(0, 1);
    switch (result.error) {
      case WATER_ERROR_SENSOR_TIMEOUT: lcd.print("Sensor Timeout"); break;
      case WATER_ERROR_SENSOR_COMMUNICATION: lcd.print("Comm Error"); break;
      case WATER_ERROR_SENSOR_INVALID_DATA: lcd.print("Invalid Data"); break;
      case WATER_ERROR_PUMP_TIMEOUT: lcd.print("Pump Timeout"); break;
      default: lcd.print("Unknown Error"); break;
    }
  } else {
    lcd.setCursor(0, 0);
    lcd.print("Water Level: ");
    lcd.print(result.level);
    lcd.print("%");
    lcd.setCursor(0, 1);
    if (result.inletPumpActive) lcd.print("Inlet Pump ON");
    else if (result.outletPumpActive) lcd.print("Outlet Pump ON");
    else lcd.print("Pumps: OK");
  }
}

int16_t getLowThreshold() { return AppState::lowThreshold; }
int16_t getHighThreshold() { return AppState::highThreshold; }

void setLowThreshold(int16_t threshold) {
  if (threshold < AppState::highThreshold) {
    AppState::lowThreshold = threshold;
    saveAppStateToConfiguration();
  }
}

void setHighThreshold(int16_t threshold) {
  if (threshold > AppState::lowThreshold && threshold <= 100) {
    AppState::highThreshold = threshold;
    saveAppStateToConfiguration();
  }
}

void setWaterThresholds(int16_t low, int16_t high) {
  if (low >= 0 && high <= 100 && low < high) {
    AppState::lowThreshold = low;
    AppState::highThreshold = high;
    saveAppStateToConfiguration();
  }
}

void getCurrentWaterLevel(uint8_t *highBuf, uint8_t *lowBuf) {
  waterSensor.getCurrentWaterLevel(highBuf, lowBuf);
}

void read_water_sensor(uint8_t *highBuf, uint8_t *lowBuf) {
  waterSensor.getCurrentWaterLevel(highBuf, lowBuf);
}

bool checkSensorHealth() {
  return waterSensor.readSensorData() == WATER_ERROR_NONE && waterSensor.isSensorConnected();
}
