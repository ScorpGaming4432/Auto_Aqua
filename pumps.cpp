/**
 * ============================================================================
 * PUMPS.CPP - Pump Control Implementation
 * ============================================================================
 * 
 * Implements pump activation and control for the aquarium system.
 */

#include "pumps.h"
#include "screens.h"
#include <Arduino.h>
#include <EEPROM.h>

// EEPROM storage addresses for pump mode settings
#define EEPROM_ADDR_INLET_MODE 50
#define EEPROM_ADDR_OUTLET_MODE 51

// Pump mode state (true = AUTO, false = MANUAL)
static bool inletPumpAuto = true;
static bool outletPumpAuto = true;

void initPumpModes() {
  inletPumpAuto = EEPROM.read(EEPROM_ADDR_INLET_MODE) ? true : false;
  outletPumpAuto = EEPROM.read(EEPROM_ADDR_OUTLET_MODE) ? true : false;

  Serial.print("[PUMPS] Inlet mode: ");
  Serial.println(inletPumpAuto ? "AUTO" : "MANUAL");
  Serial.print("[PUMPS] Outlet mode: ");
  Serial.println(outletPumpAuto ? "AUTO" : "MANUAL");
}

void toggleInletPumpMode() {
  inletPumpAuto = !inletPumpAuto;
  EEPROM.write(EEPROM_ADDR_INLET_MODE, inletPumpAuto ? 1 : 0);
  Serial.print("[PUMPS] Inlet pump mode: ");
  Serial.println(inletPumpAuto ? "AUTO" : "MANUAL");
}

void toggleOutletPumpMode() {
  outletPumpAuto = !outletPumpAuto;
  EEPROM.write(EEPROM_ADDR_OUTLET_MODE, outletPumpAuto ? 1 : 0);
  Serial.print("[PUMPS] Outlet pump mode: ");
  Serial.println(outletPumpAuto ? "AUTO" : "MANUAL");
}

bool getInletPumpMode() {
  return inletPumpAuto;
}

bool getOutletPumpMode() {
  return outletPumpAuto;
}

Pump::Pump() : amount(0), duration(0) {}

int32_t Pump::edit(uint8_t pumpIndex, const char* amountTitle) {
  return pumpAmountScreen(amountTitle, pumpIndex, true, amount);
}

int32_t Pump::viewEdit(uint8_t pumpIndex, const char* amountTitle) {
  return pumpAmountScreen(amountTitle, pumpIndex, false, amount);
}

void Pump::setAmount(uint16_t v) {
  amount = v;
}

uint16_t Pump::getAmount() const {
  return amount;
}

void Pump::setDuration(uint64_t d) {
  duration = d;
}

uint64_t Pump::getDuration() const {
  return duration;
}

void pump_work(uint8_t pump_pin, uint16_t duration_ms) {
  digitalWrite(pump_pin, HIGH);  // Activate pump (assuming active HIGH)
  delay(duration_ms);            // Run for specified duration
  digitalWrite(pump_pin, LOW);   // Deactivate pump
}
