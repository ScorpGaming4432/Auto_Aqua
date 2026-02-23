# REFACTORING PLAN - Systematic Approach to Fix All Violations

## PHASE 1: Foundation Fixes (Week 1-2)

### 1.1 Create Hardware Abstraction Layer
**Priority: CRITICAL** - This fixes hardware leakage across multiple files

**Create new files:**
- `hardware.h` - Centralized hardware definitions
- `hardware.cpp` - Hardware initialization and management

**Move these from scattered locations:**
- Pin definitions from `pumps.h`, `screens.h`, `display.h`
- Hardware constants (flow rates, timeouts, etc.)

**Files to update:**
- `pumps.h` - Remove pin definitions, use hardware abstraction
- `screens.h` - Remove pin definitions, use hardware abstraction  
- `display.h` - Remove pin definitions, use hardware abstraction
- `auto_aqua.ino` - Remove `LIGHT_PIN` reference

### 1.2 Reduce Global Variables
**Priority: HIGH** - Foundation for better architecture

**Strategy:**
- Move globals into appropriate modules (encapsulation)
- Use accessor functions where needed
- Reduce from 3+ to ≤3 per file

**Target files:**
- `auto_aqua.ino` - Move `LANG_BUFFER` to screens module
- `appstate.cpp` - Keep in namespace, but document clearly
- `display.cpp` - Keep `lcd` as it's hardware-specific
- `screens.cpp` - Keep `keypad` as it's UI-specific

## PHASE 2: Structural Refactoring (Week 3-4)

### 2.1 Split Large Files
**Priority: HIGH** - Files >500 lines need decomposition

**Target files:**
1. **`screens.cpp` (760 lines)** - Split into:
   - `screen_navigation.cpp` - Menu navigation logic
   - `screen_input.cpp` - Input handling screens
   - `screen_display.cpp` - Display-only screens
   - `screen_animation.cpp` - Splash screen, animations

2. **`water.cpp` (692 lines)** - Split into:
   - `water_sensor.cpp` - Sensor reading logic
   - `water_pumps.cpp` - Pump control logic
   - `water_calibration.cpp` - Sensor calibration

3. **`storage.cpp` (212 lines)** - Split into:
   - `eeprom_io.cpp` - Low-level EEPROM operations
   - `config_validation.cpp` - Configuration validation

### 2.2 Break Down Large Functions
**Priority: HIGH** - Functions >30 lines need decomposition

**Target functions:**
1. **`setup()` in auto_aqua.ino (227 lines)** - Split into:
   - `setupHardware()` - Initialize all hardware
   - `setupConfiguration()` - Load/validate config
   - `setupUserInterface()` - Initialize UI components

2. **`loop()` in auto_aqua.ino (127 lines)** - Split into:
   - `handleUserInput()` - Process keypad
   - `updateSystemState()` - Check sensors, pumps
   - `updateDisplay()` - Update LCD

3. **`langConfigScreen()` in screens.cpp** - Extract input handling
4. **`checkDosingSchedule()` in pumps.cpp** - Extract pump activation logic

## PHASE 3: Interface & Complexity (Week 5-6)

### 3.1 Fix Struct/Class Violations
**Priority: MEDIUM** - Improve code organization

**Target: `Language` struct (22 fields)**
- Split into smaller, focused structs:
  - `LanguageBasic` - Basic UI strings
  - `LanguageMessages` - Status/error messages
  - `LanguagePrompts` - Input prompts

**Target: `Pump` class (9+ public methods)**
- Group related methods:
  - Configuration methods (setAmount, setDuration, etc.)
  - State query methods (getAmount, getIfLet, etc.)
  - Consider using method chaining or builder pattern

### 3.2 Reduce Function Complexity
**Priority: MEDIUM** - Improve maintainability

**Techniques:**
- Extract complex conditionals into named functions
- Use early returns to reduce nesting
- Replace switch statements with polymorphism where appropriate

## PHASE 4: Code Quality (Week 7-8)

### 4.1 Magic Numbers → Constants
**Priority: LOW** - Easy wins, improve readability

**Create `constants.h`:**
```cpp
namespace Timing {
    constexpr uint16_t DISPLAY_DELAY_MS = 1000;
    constexpr uint16_t PUMP_TIMEOUT_MS = 30000;
    constexpr uint16_t SENSOR_TIMEOUT_MS = 1000;
}

namespace Limits {
    constexpr uint8_t MAX_PUMPS = 5;
    constexpr uint16_t MAX_DOSAGE_ML = 9999;
}
```

### 4.2 Replace Delays with Timers
**Priority: MEDIUM** - Improve responsiveness

**Create `timer.h/cpp`:**
- Non-blocking timer class
- Replace `delay()` calls throughout codebase

### 4.3 Include Hygiene
**Priority: LOW** - Clean up dependencies

**Actions:**
- Remove unnecessary includes
- Use forward declarations where possible
- Verify no cyclic dependencies

## PHASE 5: Testing & Validation (Week 9)

### 5.1 Test Each Phase
- Unit test individual modules after each phase
- Integration test after Phase 2
- Full system test after Phase 4

### 5.2 Validate Guidelines
- Run automated checks against coding guidelines
- Verify no regressions in functionality

## IMPLEMENTATION STRATEGY

### Week-by-Week Breakdown:
- **Week 1**: Hardware abstraction layer
- **Week 2**: Global variable reduction
- **Week 3**: Split screens.cpp and water.cpp
- **Week 4**: Split remaining large files, fix large functions
- **Week 5**: Fix Language struct, Pump class
- **Week 6**: Reduce function complexity
- **Week 7**: Magic numbers, delay replacement
- **Week 8**: Include hygiene, final cleanup
- **Week 9**: Testing and validation

### Success Metrics:
- [ ] All files <500 lines
- [ ] All functions <30 lines
- [ ] All structs <10 fields
- [ ] Global variables ≤3 per file
- [ ] No hardware leakage
- [ ] Cyclomatic complexity <10
- [ ] All tests passing

### Risk Mitigation:
1. **Backup frequently** - Use git branches for each phase
2. **Test incrementally** - Don't refactor everything at once
3. **Document changes** - Keep track of what moved where
4. **Validate functionality** - Ensure system works after each phase

This systematic approach breaks down a massive refactoring into manageable chunks, reducing risk and making progress visible.