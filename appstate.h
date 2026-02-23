/**
 * ============================================================================
 * APPSTATE.H - Global Application State
 * ============================================================================
 *
 * Centralized global state used across the firmware. These variables are
 * defined elsewhere (typically in a .cpp file) and declared here for
 * cross-module access. Keep comments concise and document units, valid ranges
 * and the purpose of each field.
 */

#ifndef APPSTATE_H
#define APPSTATE_H

#include <stdint.h>
#include "pumps.h"
#include "water.h"

namespace AppState {

// Index of the currently selected language (0-based index into language table)
extern uint8_t languageIndex;

// Array of pump configuration/state structures. Size is defined by PUMP_COUNT
// in `pumps.h`. This holds configuration and runtime state for all pumps,
// including dosing pumps, inlet/outlet pumps, and any user-configured pumps.
extern Pump pumps[PUMP_COUNT];

// Tank volume used for dosing calculations. Unit: litres. Valid range depends
// on application but typically > 0. Used by dosing routines to convert
// concentrations/volumes into pump run-times.
extern uint32_t tankVolume;

// Signed time offset (seconds) applied to the system clock/RTC to correct
// drift or to apply a user-set timezone/offset. Unit: seconds (signed).
extern int64_t timeOffset;

// Water level thresholds are percentages in the range 0-100.
// `lowThreshold` is the percentage at or below which the inlet pump should
// attempt to add water. Ensure `lowThreshold < highThreshold`.
extern uint16_t lowThreshold;

// `highThreshold` is the percentage at or above which the outlet pump should
// attempt to remove water. Ensure `highThreshold > lowThreshold`.
extern uint16_t highThreshold;

} // namespace AppState

#endif // APPSTATE_H
