/**
 * ============================================================================
 * WATER_CONTROL.CPP - Water Control and Pump Management
 * ============================================================================
 */

#include "water.h"
#include "hardware.h"
#include "appstate.h"
#include "storage.h"
#include "debug.h"
#include "pumps.h"
#include <Arduino.h>

extern WaterSensor waterSensor;
static WaterPumpState pumpState;

void initWaterManagement() {
  pinMode(Hardware::INLET_PUMP_PIN, OUTPUT);
  pinMode(Hardware::OUTLET_PUMP_PIN, OUTPUT);
  pinMode(Hardware::ELECTROVALVE_PIN, OUTPUT);
  digitalWrite(Hardware::INLET_PUMP_PIN, HIGH);
  digitalWrite(Hardware::OUTLET_PUMP_PIN, HIGH);
  digitalWrite(Hardware::ELECTROVALVE_PIN, HIGH);

  initPumpModes();

  if (AppState::lowThreshold >= AppState::highThreshold) {
    AppState::lowThreshold = 30;
    AppState::highThreshold = 70;
  }
}

void runPumpSafely(uint8_t pumpPin, uint16_t duration) {
  if (pumpState.pumpActive) return;

  controlElectrovalve(true);
  delay(500);

  if (duration > Hardware::MAX_PUMP_RUN_TIME_MS) duration = Hardware::MAX_PUMP_RUN_TIME_MS;

  pumpState.pumpActive = true;
  pumpState.activePumpPin = pumpPin;
  pumpState.pumpStartTime = millis();

  if (pumpPin == Hardware::INLET_PUMP_PIN) pumpState.inletPumpRunning = true;
  else if (pumpPin == Hardware::OUTLET_PUMP_PIN) pumpState.outletPumpRunning = true;

  digitalWrite(pumpPin, LOW);

  if (pumpPin == Hardware::INLET_PUMP_PIN) pumpState.inletPumpTotalRuntime += duration;
  else if (pumpPin == Hardware::OUTLET_PUMP_PIN) pumpState.outletPumpTotalRuntime += duration;

  unsigned long startWait = millis();
  while (millis() - startWait < duration) {
    if (millis() - pumpState.pumpStartTime > Hardware::MAX_PUMP_RUN_TIME_MS) break;
    delay(100);
  }

  digitalWrite(pumpPin, HIGH);
  pumpState.pumpActive = false;
  controlElectrovalve(false);

  if (pumpPin == Hardware::INLET_PUMP_PIN) {
    pumpState.inletPumpWasActive = false;
    pumpState.inletPumpRunning = false;
  } else if (pumpPin == Hardware::OUTLET_PUMP_PIN) {
    pumpState.outletPumpWasActive = false;
    pumpState.outletPumpRunning = false;
  }
}

WaterLevelResult checkWaterLevel() {
  WaterError error = waterSensor.readSensorData();
  if (error != WATER_ERROR_NONE) {
    pumpState.currentError = error;
    return { error, 0, pumpState.inletPumpRunning, pumpState.outletPumpRunning };
  }
  if (!waterSensor.isSensorConnected()) {
    return { WATER_ERROR_SENSOR_TIMEOUT, 0, pumpState.inletPumpRunning, pumpState.outletPumpRunning };
  }

  uint8_t currentLevel = waterSensor.calculateWaterLevel();

  if (currentLevel < AppState::lowThreshold - Hardware::HYSTERESIS_MARGIN_PERCENT) {
    if (!pumpState.inletPumpWasActive) {
      pumpState.inletPumpWasActive = true;
      digitalWrite(Hardware::ELECTROVALVE_PIN, LOW);
      digitalWrite(Hardware::INLET_PUMP_PIN, LOW);
      pumpState.inletPumpRunning = true;
      while (waterSensor.calculateWaterLevel() < AppState::lowThreshold) {
        delay(100);
      }
      digitalWrite(Hardware::INLET_PUMP_PIN, HIGH);
      pumpState.inletPumpRunning = false;
      digitalWrite(Hardware::ELECTROVALVE_PIN, HIGH);
      pumpState.inletPumpWasActive = false;
    }
  }

  if (currentLevel > AppState::highThreshold + Hardware::HYSTERESIS_MARGIN_PERCENT) {
    if (!pumpState.outletPumpWasActive) {
      pumpState.outletPumpWasActive = true;
      digitalWrite(Hardware::OUTLET_PUMP_PIN, LOW);
      pumpState.outletPumpRunning = true;
      while (waterSensor.calculateWaterLevel() > AppState::highThreshold) {
        delay(100);
      }
      digitalWrite(Hardware::OUTLET_PUMP_PIN, HIGH);
      pumpState.outletPumpRunning = false;
      pumpState.outletPumpWasActive = false;
    }
  }

  return { WATER_ERROR_NONE, currentLevel, pumpState.inletPumpRunning, pumpState.outletPumpRunning };
}

void controlElectrovalve(bool open) {
  if (open) {
    digitalWrite(Hardware::ELECTROVALVE_PIN, LOW);
    pumpState.electrovalveActive = true;
  } else {
    digitalWrite(Hardware::ELECTROVALVE_PIN, HIGH);
    pumpState.electrovalveActive = false;
  }
}

bool isElectrovalveOpen() { return pumpState.electrovalveActive; }

void emergencyStopLetPumps() {
  digitalWrite(Hardware::INLET_PUMP_PIN, HIGH);
  digitalWrite(Hardware::OUTLET_PUMP_PIN, HIGH);
  pumpState.pumpActive = false;
}

void getPumpStatistics(uint32_t *inletRuntime, uint32_t *outletRuntime) {
  if (inletRuntime != NULL) *inletRuntime = pumpState.inletPumpTotalRuntime;
  if (outletRuntime != NULL) *outletRuntime = pumpState.outletPumpTotalRuntime;
}

void resetPumpStatistics() {
  pumpState.inletPumpTotalRuntime = 0;
  pumpState.outletPumpTotalRuntime = 0;
}

WaterError getWaterError() { return pumpState.currentError; }
void clearWaterError() { pumpState.currentError = WATER_ERROR_NONE; }
