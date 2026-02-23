# Phase 2: Global Variable Reduction - Execution Plan

## Current Global Variables Analysis

### auto_aqua.ino (3 globals - AT LIMIT)
- `lightofft` (uint64_t)
- `lightont` (uint64_t)
- `resetFunc` (function pointer)
- `LANG_BUFFER` (Language struct - 4th one, VIOLATION!)

**Status**: ❌ Exceeds limit (4 > 3)

### screens.cpp (2 globals - OK but can improve)
- `keypad` (Keypad object)
- `editFlag` (bool)

**Status**: ✅ Within limit (2 ≤ 3)

### display.cpp (2 globals - OK but hardware-specific)
- `lcd` (LiquidCrystal_I2C)
- `dimTimer` (uint32_t)

**Status**: ✅ Within limit (hardware-specific, acceptable)

### appstate.cpp (6 globals in namespace - WITHIN limit due to namespace)
- Stored in `AppState::` namespace
- `languageIndex`, `pumps[]`, `tankVolume`, `timeOffset`, `lowThreshold`, `highThreshold`

**Status**: ✅ Properly namespaced

### water.cpp (11 static variables - CRITICAL VIOLATION!)
- `inletPumpTotalRuntime`, `outletPumpTotalRuntime`, `pumpStartTime`, `pumpActive`, `activePumpPin`
- `currentError`, `inletPumpWasActive`, `outletPumpWasActive`
- `inletPumpRunning`, `outletPumpRunning`
- `electrovalveActive`, `electrovalveTotalRuntime`, `waterSensor`

**Status**: ❌ CRITICAL (13 static variables!)

## Refactoring Strategy

### 1. LANG_BUFFER (auto_aqua.ino)
**Problem**: 4th global variable
**Solution**: Move to screens module as it's UI-specific
**Impact**: Minor - used for language data in setup() and loop()

### 2. Water Pump State (water.cpp)
**Problem**: 13 static variables scattered
**Solution**: Create a `WaterPumpState` struct to consolidate state
**Impact**: Reduces from 13 to 1 variable

### 3. Light Control (auto_aqua.ino)
**Problem**: `lightofft`, `lightont` are separate
**Solution**: Keep as-is (essential for functionality) - accept as necessary

## Implementation Priority
1. Create `WaterPumpState` struct (consolidate 13 → 1)
2. Move `LANG_BUFFER` to screens module
3. Verify all files are ≤3 non-const globals

## Expected Result
- auto_aqua.ino: 3 globals ✓
- screens.cpp: 3 globals ✓
- display.cpp: 2 globals ✓
- water.cpp: 1 struct global ✓
- appstate.cpp: namespaced ✓
