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
    lcdPrintWithGlyphs(LANG_BUFFER.error.waterSensorError, LANG_WATER_ERROR_LEN, 0, 0);
    lcd.setCursor(0, 1);
    switch (result.error) {
      case WATER_ERROR_SENSOR_TIMEOUT: 
        lcdPrintWithGlyphs(LANG_BUFFER.error.sensorTimeout, LANG_WATER_ERROR_LEN, 0, 1); 
        break;
      case WATER_ERROR_SENSOR_COMMUNICATION: 
        lcdPrintWithGlyphs(LANG_BUFFER.error.commError, LANG_WATER_ERROR_LEN, 0, 1); 
        break;
      case WATER_ERROR_SENSOR_INVALID_DATA: 
        lcdPrintWithGlyphs(LANG_BUFFER.error.invalidData, LANG_WATER_ERROR_LEN, 0, 1); 
        break;
      case WATER_ERROR_PUMP_TIMEOUT: 
        lcdPrintWithGlyphs(LANG_BUFFER.error.pumpTimeout, LANG_WATER_ERROR_LEN, 0, 1); 
        break;
      default: 
        lcdPrintWithGlyphs(LANG_BUFFER.error.unknownError, LANG_WATER_ERROR_LEN, 0, 1); 
        break;
    }
  } else {
    lcdPrintWithGlyphs(LANG_BUFFER.status.waterLevel, LANG_WATER_ERROR_LEN, 0, 0);
    lcd.print(result.level);
    lcd.print("%");
    lcd.setCursor(0, 1);
    if (result.inletPumpActive) 
      lcdPrintWithGlyphs(LANG_BUFFER.status.inletPumpOn, LANG_PUMP_STATUS_LEN, 0, 1);
    else if (result.outletPumpActive) 
      lcdPrintWithGlyphs(LANG_BUFFER.status.outletPumpOn, LANG_PUMP_STATUS_LEN, 0, 1);
    else 
      lcdPrintWithGlyphs(LANG_BUFFER.status.pumpsOk, LANG_PUMP_STATUS_LEN, 0, 1);
  }
}

int16_t getLowThreshold() { return AppState::lowThreshold; }
int16_t getHighThreshold() { return AppState::highThreshold; }

void setLowThreshold(uint16_t threshold) {
  if (threshold < AppState::highThreshold) {
    AppState::lowThreshold = threshold;
    saveAppStateToConfiguration();
  }
}

void setHighThreshold(uint16_t threshold) {
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
