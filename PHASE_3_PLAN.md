# Phase 3: Function Decomposition Plan

## Goal
Break down setup() (66 lines) and loop() (163 lines) to comply with 30-line limit.

## Current Status
- **setup()**: 66 lines → VIOLATES limit by 36 lines
- **loop()**: 163 lines → VIOLATES limit by 133 lines

## Decomposition Strategy

### setup() → 5 helper functions
1. `setupSerial()` - Serial and I2C initialization
2. `setupInitialScreen()` - Splash screen
3. `runInitialConfiguration()` - Full setup wizard if needed
4. `loadSavedConfiguration()` - Load from EEPROM and apply
5. `initializeSystem()` - Water management and final init

### loop() → 6 handler functions
1. `displayMainScreen()` - LCD display management
2. `handlePumpConfiguration(char k)` - Keys 1-6 for pump editing
3. `handleWaterManagement(char k)` - Keys A, C for water features
4. `handleSystemConfiguration(char k)` - Keys 0, B for language/time
5. `handleFactoryReset(char k)` - Key * for reset
6. `handleWaterMonitoring()` - Automatic water checks and light control

## Expected Outcome
- setup(): ~12 lines + 5 helpers (10-15 lines each)
- loop(): ~15 lines + 6 handlers (15-25 lines each)
- All functions within 30-line limit
- Functionality completely preserved
