/**
 * ============================================================================
 * PUMPS.CPP - Pump Control Implementation
 * ============================================================================
 */

#include "appstate.h"
#include "debug.hpp"
#include "pumps.h"
#include "screens.h"
#include "water.h"
#include <Arduino.h>

void initPumpModes() {
  if (Hardware::PUMP_COUNT >= 5) {
    AppState::pumps[3].setRole(PumpRole::INLET);
    AppState::pumps[4].setRole(PumpRole::OUTLET);
  }
}

static uint8_t pumpIndexToPin(uint8_t pumpIndex) {
  if (pumpIndex < Hardware::DOSING_PUMP_COUNT) {
    return Hardware::DOSING_PUMP_PINS[pumpIndex];
  }
  return 0;
}

void checkDosingSchedule() {
  uint64_t now = seconds();
  for (uint8_t i = 0; i < Hardware::DOSING_PUMP_COUNT; ++i) {
    Pump& p = AppState::pumps[i];
    if (p.getRole() != PumpRole::DOSING) {
      SerialPrint(PUMPS, "ERROR:", NOT_CORRECT, "\nPUMP OF INDEX ", i, "SHOULD HAVE BEEN DOSING. CURRENT IS DIFFERENT");
      continue;
    }
    DosingConfig cfg = p.getConfig();
    if (cfg.interval == 0 || !p.shouldDose(now))
      continue;

    if (cfg.amount == 0) {
      cfg.lastTime = now;
      p.setConfig(cfg);
      continue;
    }

    uint64_t durationMs = (static_cast<uint64_t>(cfg.amount) * 1000ULL) /
                          static_cast<uint64_t>(Hardware::PUMP_FLOW_RATE_ML_PER_SEC);
    if (durationMs > Hardware::MAX_PUMP_RUN_TIME_MS)
      durationMs = Hardware::MAX_PUMP_RUN_TIME_MS;

    cfg.duration = durationMs;
    cfg.lastTime = now;
    p.setConfig(cfg);

    uint8_t pin = pumpIndexToPin(i);
    runPumpSafely(pin, static_cast<uint16_t>(durationMs));
  }
}

Pump::Pump() {}

int32_t Pump::edit(uint8_t pumpIndex, const char* amountTitle) {
  if (role != PumpRole::DOSING)
    return -1;
  return pumpAmountScreen(amountTitle, pumpIndex, true, config.amount);
}

int32_t Pump::viewEdit(uint8_t pumpIndex, const char* amountTitle) {
  if (role != PumpRole::DOSING)
    return -1;
  return pumpAmountScreen(amountTitle, pumpIndex, false, config.amount);
}

void Pump::setConfig(const DosingConfig& c) { config = c; }
DosingConfig Pump::getConfig() const { return config; }

void Pump::setRole(PumpRole r) { role = r; }
PumpRole Pump::getRole() const { return role; }

bool Pump::shouldDose(uint64_t currentSeconds) const {
  if (config.interval == 0)
    return false;
  uint64_t intervalSeconds = static_cast<uint64_t>(config.interval) * 86400ULL;
  if (currentSeconds < config.lastTime)
    return true;
  return (currentSeconds - config.lastTime) >= intervalSeconds;
}
