/**
 * ============================================================================
 * APPSTATE.CPP - Global Application State Implementation
 * ============================================================================
 * 
 * Defines global state for the aquarium system.
 */

#include "appstate.h"

namespace AppState {
  uint8_t languageIndex = 0;
  Pump pumps[PUMP_COUNT];
  uint32_t tankVolume = 0;
  int64_t timeOffset = 0;
  uint8_t lowThreshold = 100;
  uint8_t highThreshold = 0;
}
