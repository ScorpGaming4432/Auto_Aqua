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

// Pump flow rate constant: 2000 ml/ms (adjust based on actual pump specifications)
#define PRZEPLYW 2000

class Pump {
public:
  Pump();

  int32_t edit(uint8_t pumpIndex, const char* amountTitle);
  int32_t viewEdit(uint8_t pumpIndex, const char* amountTitle);

  void setAmount(uint16_t v);
  uint16_t getAmount() const;

  void setDuration(uint64_t d);
  uint64_t getDuration() const;

private:
  uint16_t amount;
  uint64_t duration;  // Added duration field
};


/**
 * Activate a pump for a specified duration
 * @param pump_pin Digital pin connected to the pump relay/control
 * @param duration_ms How long to run the pump in milliseconds
 */
void pump_work(uint8_t pump_pin, uint16_t duration_ms);

#endif