/**
 * ============================================================================
 * PUMPS.CPP - Pump Control Implementation
 * ============================================================================
 * 
 * Implements pump activation and control for the aquarium system.
 */

#include "pumps.h"
#include <Arduino.h>

void pump_work(uint8_t pump_pin, uint16_t duration_ms) {
  digitalWrite(pump_pin, HIGH);  // Activate pump (assuming active HIGH)
  delay(duration_ms);            // Run for specified duration
  digitalWrite(pump_pin, LOW);   // Deactivate pump
}
