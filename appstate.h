/**
 * ============================================================================
 * APPSTATE.H - Global Application State
 * ============================================================================
 * 
 * Centralized global state management for the aquarium system.
 */

#ifndef APPSTATE_H
#define APPSTATE_H

#include <stdint.h>
#include "pumps.h"
#include "water.h"

namespace AppState {
  extern uint8_t languageIndex;
  extern Pump pumps[PUMP_COUNT];
  extern uint32_t tankVolume;
  extern int64_t timeOffset;
  extern uint8_t lowThreshold;
  extern uint8_t highThreshold;
}

#endif
