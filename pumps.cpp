// 2 - 7 piny do styczników


// 8 - światełko
// 7 Elektrozawór
// 6, 5 - piny do wpływu i wypływu
// 2, 3, 4 - płyny

/**
 * ============================================================================
 * PUMPS.CPP - Pump Control Implementation
 * ============================================================================
 * 
 * Implements pump activation and control for the aquarium system.
 */

#include "pumps.h"
#include "screens.h"
#include "appstate.h"
#include "water.h"
#include <Arduino.h>
#include <EEPROM.h>


// Initialize pump configurations
void initPumpModes() {
  AppState::pumps[INLET_PUMP_PIN].setInlet(true);    // Inlet pump - always automatic
  AppState::pumps[OUTLET_PUMP_PIN].setOutlet(true);  // Outlet pump - always automatic
}

// Map logical pump index (0..PUMP_COUNT-3) to physical pin numbers
static uint8_t pumpIndexToPin(uint8_t pumpIndex) {
  // Dosing pumps are wired to pins 2,3,4 (pumpIndex 0->2, 1->3, 2->4)
  return (uint8_t)(2 + pumpIndex);
}

// Scheduler: check dosing pumps and run them when interval elapsed
void checkDosingSchedule() {
  uint64_t now = seconds();

  // Dosing pumps are the first PUMP_COUNT-2 pumps
  for (uint8_t i = 0; i < PUMP_COUNT - 2; ++i) {
    Pump &p = AppState::pumps[i];

    // Skip if marked as let pump or interval disabled
    if (p.getIfLet()) continue;
    uint32_t intervalHours = p.getDosingInterval();
    if (intervalHours == 0) continue;

    if (!p.shouldDose(now)) continue;

    uint16_t amount = p.getAmount();
    if (amount == 0) {
      // Nothing to dose; update last dosing time to avoid repeated checks
      p.setLastDosingTime(now);
      continue;
    }

    // Calculate duration in milliseconds using flowrate PRZEPLYW (ml/s)
    // duration_ms = (amount_ml * 1000) / flow_ml_per_s
    uint64_t durationMs = ((uint64_t)amount * 1000ULL) / (uint64_t)PRZEPLYW;
    if (durationMs > MAX_PUMP_RUN_TIME_MS) durationMs = MAX_PUMP_RUN_TIME_MS;

    p.setDuration(durationMs);
    p.setLastDosingTime(now);

    uint8_t pin = pumpIndexToPin(i);
    Serial.print("[PUMPS] Scheduled dosing pump "); Serial.print(i);
    Serial.print(" on pin "); Serial.print(pin);
    Serial.print(" for "); Serial.print((unsigned long)durationMs); Serial.println(" ms");

    // Run safely (this will open/close electrovalve via runPumpSafely)
    runPumpSafely(pin, (uint16_t)durationMs);
  }
}

Pump::Pump()
  : amount(0), duration(0) {}

int32_t Pump::edit(uint8_t pumpIndex, const char* amountTitle) {
  if (isLet) {
    // Let pumps are not editable by user
    return -1;
  }
  return pumpAmountScreen(amountTitle, pumpIndex, true, amount);
}

int32_t Pump::viewEdit(uint8_t pumpIndex, const char* amountTitle) {
  if (isLet) {
    // Let pumps are not viewable by user
    return -1;
  }
  return pumpAmountScreen(amountTitle, pumpIndex, false, amount);
}

void Pump::setAmount(uint16_t v) {
  if (isLet) {
    // Let pumps amount is not settable by user
    return;
  }
  amount = v;
}

uint16_t Pump::getAmount() const {
  if (isLet) {
    // Let pumps amount is not viewable by user
    return 0;
  }
  return amount;
}

void Pump::setDuration(uint64_t d) {
  if (isLet) {
    // Let pumps duration is not settable by user
    return;
  }
  duration = d;
}

uint64_t Pump::getDuration() const {
  if (isLet) {
    // Let pumps duration is not viewable by user
    return 0;
  }
  return duration;
}

void Pump::setOutlet(bool value) {
  isLet = true;
  isOutLet = value;
}

void Pump::setInlet(bool value) {
  isLet = true;
  isOutLet = !value;
}

bool Pump::getIfOutlet() const {
  return isOutLet && isLet;
}

bool Pump::getIfInlet() const {
  return !isOutLet && isLet;
}

bool Pump::getIfLet() const {
  return isLet;
}

void Pump::setDosingInterval(uint64_t interval) {
  dosingInterval = interval;
}

uint32_t Pump::getDosingInterval() const {
  return dosingInterval;
}

void Pump::setLastDosingTime(uint64_t time) {
  lastDosingTime = time;
}

uint64_t Pump::getLastDosingTime() const {
  return lastDosingTime;
}

bool Pump::shouldDose(uint64_t currentSeconds) const {
  if (dosingInterval == 0) return false;
  uint64_t intervalSeconds = (uint64_t)dosingInterval * 86400ULL; // days -> seconds
  if (currentSeconds < lastDosingTime) return true; // clock reset or never set -> allow
  return (currentSeconds - lastDosingTime) >= intervalSeconds;
}

void pumpWork(uint8_t pump_pin, uint16_t duration_ms) {
  digitalWrite(pump_pin, LOW);  // Activate pump (assuming active LOW)
  delay(duration_ms);            // Run for specified duration
  digitalWrite(pump_pin, HIGH);   // Deactivate pump
}

// void runPumpSafely(uint8_t pump_pin, uint16_t duration_ms, uint16_t amount) {
//   uint8_t originalSensorData = getTouchedSections();
//   digitalWrite(pump_pin, HIGH);  // Activate pump (assuming active HIGH)
//   while (getTouchedSections() - originalSensorData < amount/AppState::tankVolume && duration_ms > 0) {
//     delay(100);  // Check every 100ms
//     duration_ms -= 100;
//   } else {
//     digitalWrite(pump_pin, LOW);   // Deactivate pump
//     Serial.println("[PUMPS] Pump run stopped due to sensor feedback or duration limit");
//   }
// }
