/**
 * ============================================================================
 * APPSTATE.CPP - Global Application State Implementation
 * ============================================================================
 *
 * Defines global state for the aquarium system.
 */

#include "appstate.h"
#include <Arduino.h>
#include "hardware.h"
#include "storage.h"
#include <stdint.h>

namespace AppState {
uint8_t languageIndex = 0;
Pump pumps[Hardware::PUMP_COUNT];
uint32_t tankVolume = 0;
int64_t timeOffset = 0;
uint16_t lowThreshold = UNSET_U16;
uint16_t highThreshold = UNSET_U16;
uint64_t lightOffTime = 0;
uint64_t lightOnTime = 0;
uint8_t lightState = HIGH; // Start with light OFF
bool lightOverrideActive = false;

// cleaning interval state
uint16_t waterCleaningIntervalDays = 0; // days between cycles
uint64_t lastCleaningTime = 0; // epoch seconds of last cycle
}
