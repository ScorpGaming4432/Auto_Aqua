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

// Pump pin assignments (adjust for your hardware)
#define INLET_PUMP_PIN 7
#define OUTLET_PUMP_PIN 6
#define ELECTROVALVE_PIN 5

// Pump flow rate constant: 2 ml/s (adjust based on actual pump specifications)
#define PRZEPLYW 2

class Pump {
public:
  Pump();

  int32_t edit(uint8_t pumpIndex, const char* amountTitle);
  int32_t viewEdit(uint8_t pumpIndex, const char* amountTitle);

  void setAmount(uint16_t v);
  uint16_t getAmount() const;

  void setDuration(uint64_t d);  // calculate
  uint64_t getDuration() const;

  void setOutlet(bool value);
  bool getIfOutlet() const;

  void setInlet(bool value);
  bool getIfInlet() const;

  bool getIfLet() const;

  // Automatic dosing functionality
  void setDosingInterval(uint32_t interval);
  uint32_t getDosingInterval() const;
  void setLastDosingTime(uint64_t time);
  uint64_t getLastDosingTime() const;
  bool shouldDose(uint64_t currentSeconds) const;

private:
  uint16_t amount;
  uint64_t duration;           // Calculated duration based on amount and flow rate
  bool isLet = false;          // True = in/out let pump, False = dosing pump
  bool isOutLet;               // True = outlet pump, False = inlet pump (only relevant if isLet is true)
  uint32_t dosingInterval = 0; // Interval between doses in hours (0 = disabled)
  uint64_t lastDosingTime = 0; // Last time this pump was activated (seconds)
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