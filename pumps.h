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

/**
 * Activate a pump for a specified duration
 * @param pump_pin Digital pin connected to the pump relay/control
 * @param duration_ms How long to run the pump in milliseconds
 */
void pump_work(uint8_t pump_pin, uint16_t duration_ms);

#endif