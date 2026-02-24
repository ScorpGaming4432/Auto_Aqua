/**
 * ============================================================================
 * UI_STATE.H - Non-blocking UI State Machine Definitions
 * ============================================================================
 */

#ifndef UI_STATE_H
#define UI_STATE_H

#include <stdint.h>

enum class UIState : uint8_t {
  MAIN_SCREEN,
  LANG_CONFIG,
  TANK_VOLUME_VIEW,
  TANK_VOLUME_EDIT,
  PUMP_CONFIG_VIEW,
  PUMP_CONFIG_EDIT_AMOUNT,
  PUMP_CONFIG_EDIT_INTERVAL,
  THRESHOLD_CONFIG_LOW,
  THRESHOLD_CONFIG_HIGH,
  TIME_DISPLAY,
  FACTORY_RESET_CONFIRM
};

namespace UIStateController {
  extern UIState currentState;
  extern uint8_t currentPumpIndex;
  
  void transitionTo(UIState newState);
  void handleCurrentState(char key);
}

#endif
