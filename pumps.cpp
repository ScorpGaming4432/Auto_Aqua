// 2 - 7 piny do styczników


// 8 - światełko
// 6 i 7 Wpływ i Wypływ
// 5 - elektrozawór
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
  AppState::pumps[-1].setInlet(true);  // Inlet pump - always automatic
  AppState::pumps[-2].setOutlet(true); // Outlet pump - always automatic
}

Pump::Pump() : amount(0), duration(0) {}

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

void pumpWork(uint8_t pump_pin, uint16_t duration_ms) {
  digitalWrite(pump_pin, HIGH);  // Activate pump (assuming active HIGH)
  delay(duration_ms);            // Run for specified duration
  digitalWrite(pump_pin, LOW);   // Deactivate pump
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
