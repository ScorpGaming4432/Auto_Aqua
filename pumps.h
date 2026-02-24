/**
 * ============================================================================
 * PUMPS.H - Pump Control Interface
 * ============================================================================
 *
 * Defines pump control functions and constants for the aquarium system.
 * Supports operation of multiple peristaltic pumps for liquid dispensing.
 */

#ifndef PUMPS_H
#define PUMPS_H

#include <stdint.h>
#include "hardware.h"

enum class PumpRole {
  DOSING,
  INLET,
  OUTLET
};

struct DosingConfig {
  uint16_t amount = 0;
  uint64_t duration = 0;
  uint64_t interval = 0;
  uint64_t lastTime = 0;
};

class Pump {
public:
  Pump();

  int32_t edit(uint8_t pumpIndex, const char* amountTitle);
  int32_t viewEdit(uint8_t pumpIndex, const char* amountTitle);

  void setConfig(const DosingConfig& config);
  DosingConfig getConfig() const;

  void setRole(PumpRole role);
  PumpRole getRole() const;

  bool shouldDose(uint64_t currentSeconds) const;

private:
  DosingConfig config;
  PumpRole role = PumpRole::DOSING;
};


/**
 * Initialize pump modes for inlet and outlet pumps
 */
void initPumpModes();

/**
 * Activate a pump for a specified duration
 * @param pump_pin Digital pin connected to the pump relay/control
 * @param duration_ms How long to run the pump in milliseconds
 */
void pumpWork(uint8_t pump_pin, uint16_t duration_ms);
void runPumpSafely(uint8_t pump_pin, uint16_t duration_ms);

// Periodic scheduler for automatic dosing pumps
void checkDosingSchedule();

#endif
