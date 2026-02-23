# CODING GUIDELINES VIOLATIONS REPORT

## auto_aqua.ino

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Line 1 | TODO comment contains inappropriate language | 8.2 Formatting Enforcement |
| Lines 21-22 | Global non-const variables `lightofft`, `lightont` exceed limit of 3 | 6.2 Global Variable Rule |
| Line 24 | Hardware pin reference `LIGHT_PIN` violates hardware leakage rule | 6.4 Hardware Leakage |
| Line 26 | Raw function pointer assignment violates C++ modern discipline | 5.1 Raw new/delete |
| Lines 28-36 | Header includes exceed limit of 8 | 4.3 Include Hygiene |
| Line 38 | Global variable `LANG_BUFFER` exceeds limit of 3 | 6.2 Global Variable Rule |
| Lines 50-277 | `setup()` function exceeds 30-line limit (227 lines) | 1.1 Function Length |
| Lines 50-277 | `setup()` function contains complex logic violating .ino discipline | 6.1 .ino Discipline |
| Lines 50-277 | `setup()` function performs multiple responsibilities | 2.3 Multiple Responsibility Heuristic |
| Lines 150-277 | `loop()` function exceeds 30-line limit (127 lines) | 1.1 Function Length |
| Lines 150-277 | `loop()` function contains complex logic violating .ino discipline | 6.1 .ino Discipline |
| Lines 150-277 | `loop()` function performs multiple responsibilities | 2.3 Multiple Responsibility Heuristic |
| Lines 184, 196, 212, etc. | Multiple `delay()` calls violate non-blocking timer preference | 6.3 Delay Usage |
| Lines 184, 196, 212 | Magic numbers used without `constexpr` definition | 7.3 Magic Numbers |

## appstate.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 21-39 | Struct fields exceed limit of 10 (6 fields) | 3.1 Field Count |
| Line 21 | Mixed domain fields (language, pumps, water) | 3.2 Mixed Responsibility Fields |

## pumps.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 15-17 | Hardware pin definitions violate hardware leakage rule | 6.4 Hardware Leakage |
| Line 19 | Macro `PRZEPLYW` violates macro abuse limit | 5.3 Macro Abuse |
| Lines 21-40 | Class `Pump` has 9 public methods, violating encapsulation | 3.4 Public Data in Classes |
| Lines 21-40 | Multiple boolean parameters in methods violate readability | 2.2 Boolean Parameter Rule |
| Lines 50-56 | Struct fields exceed limit of 10 (6 fields) | 3.1 Field Count |
| Lines 50-56 | Mixed responsibility fields (amount, duration, flags) | 3.2 Mixed Responsibility Fields |

## water.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Line 15 | Macro `PUMP_COUNT` violates macro abuse limit | 5.3 Macro Abuse |
| Lines 17-23 | Enum `WaterError` exceeds recommended complexity | 7.1 Cyclomatic Complexity |
| Lines 25-26 | Magic numbers used without `constexpr` definition | 7.3 Magic Numbers |
| Lines 28-50 | Class `WaterSensor` has complex interface | 2.3 Multiple Responsibility Heuristic |
| Lines 28-50 | Class `WaterSensor` exceeds field count limit | 3.1 Field Count |

## screens.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Line 1-760 | File exceeds 500-line limit (760 lines) | 1.4 File Length |
| Lines 28-36 | Header includes exceed limit of 8 | 4.3 Include Hygiene |
| Lines 38-39 | Global variables `keypad`, `editFlag` exceed limit of 3 | 6.2 Global Variable Rule |
| Lines 100-200 | `langConfigScreen()` function exceeds 30-line limit | 1.1 Function Length |
| Lines 100-200 | `langConfigScreen()` has high cyclomatic complexity | 7.1 Cyclomatic Complexity |
| Lines 100-200 | `langConfigScreen()` performs multiple responsibilities | 2.3 Multiple Responsibility Heuristic |

## storage.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 1-212 | File exceeds 500-line limit (212 lines) | 1.4 File Length |
| Lines 28-36 | Header includes exceed limit of 8 | 4.3 Include Hygiene |
| Lines 38-100 | `isConfigurationValid()` function exceeds 30-line limit | 1.1 Function Length |
| Lines 38-100 | `isConfigurationValid()` has high cyclomatic complexity | 7.1 Cyclomatic Complexity |

## pumps.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 1-9 | Polish comments violate language consistency | 8.1 Indentation Consistency |
| Lines 1-9 | Hardware pin references violate hardware leakage rule | 6.4 Hardware Leakage |
| Lines 28-189 | File exceeds 500-line limit (189 lines) | 1.4 File Length |
| Lines 28-189 | `checkDosingSchedule()` function exceeds 30-line limit | 1.1 Function Length |
| Lines 28-189 | `checkDosingSchedule()` has high cyclomatic complexity | 7.1 Cyclomatic Complexity |

## water.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 1-692 | File exceeds 500-line limit (692 lines) | 1.4 File Length |
| Lines 28-36 | Header includes exceed limit of 8 | 4.3 Include Hygiene |
| Lines 38-100 | Multiple global variables exceed limit of 3 | 6.2 Global Variable Rule |
| Lines 38-100 | `WaterSensor::readSensorData()` exceeds 30-line limit | 1.1 Function Length |

## display.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 8-9 | Global variables `lcd`, `dimTimer` exceed limit of 3 | 6.2 Global Variable Rule |

## debug.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 8-9 | Macro `DEBUG_SERIAL_ENABLED` violates macro abuse limit | 5.3 Macro Abuse |
| Lines 11-23 | Enum `Location` has 11 values, approaching complexity limit | 7.1 Cyclomatic Complexity |
| Lines 25-30 | Enum `Errors` has 5 values | 7.1 Cyclomatic Complexity |
| Lines 32-50 | Inline function `printLocationTag()` has high cyclomatic complexity | 7.1 Cyclomatic Complexity |

## debug.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| (No violations - file is intentionally empty) | | |

## display.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 15-16 | Macros `SCREEN_LOCATION`, `SCREEN_WIDTH`, `SCREEN_HEIGHT` violate macro abuse limit | 5.3 Macro Abuse |
| Lines 18-19 | Global variables `lcd`, `dimTimer` exceed limit of 3 | 6.2 Global Variable Rule |
| Lines 18-19 | Hardware pin references violate hardware leakage rule | 6.4 Hardware Leakage |

## input.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| (No violations - file is minimal interface) | | |

## input.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| (No violations - file is minimal implementation) | | |

## language.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 20-30 | Multiple `constexpr` definitions violate macro abuse limit | 5.3 Macro Abuse |
| Lines 32-110 | Struct `Language` has 22 fields, exceeding limit of 10 | 3.1 Field Count |
| Lines 32-110 | Struct `Language` has mixed responsibility fields (UI strings) | 3.2 Mixed Responsibility Fields |
| Lines 32-110 | Struct declaration lines exceed 120 characters | 3.3 Width Rule |

## chars.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 30-31 | Macro `LCD_AVAILABLE_CHAR_SLOTS` violates macro abuse limit | 5.3 Macro Abuse |
| Lines 33-38 | Struct `LanguageGlyphSet` has mixed responsibility fields | 3.2 Mixed Responsibility Fields |
| Lines 40-50 | Multiple global constants | 6.2 Global Variable Rule |

## storage.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 15-25 | Struct `Configuration` has 8 fields, approaching limit of 10 | 3.1 Field Count |
| Lines 15-25 | Struct `Configuration` has mixed responsibility fields | 3.2 Mixed Responsibility Fields |
| Lines 27-40 | Commented-out namespace code violates code cleanliness | 8.2 Formatting Enforcement |
| Lines 42-53 | Struct `DEFAULT_CONFIG` has 8 fields | 3.1 Field Count |

## screens.h

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 15-18 | Hardware pin references violate hardware leakage rule | 6.4 Hardware Leakage |
| Lines 20-21 | Global variables `keypad`, `editFlag` exceed limit of 3 | 6.2 Global Variable Rule |
| Lines 23-184 | Function declarations exceed 30-line limit for interface | 1.1 Function Length |
| Lines 23-184 | Multiple boolean parameters in function declarations | 2.2 Boolean Parameter Rule |

## appstate.cpp

| Line/Range | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Lines 8-13 | Global variables exceed limit of 3 (6 variables) | 6.2 Global Variable Rule |

## ARCHITECTURE VIOLATIONS

| File/Module | Violation Description | Guideline Section |
|------------|---------------------|------------------|
| Multiple files | "God Module" detection: files contain logic from multiple domains | 9.1 God Module |
| Multiple files | Cyclic include dependencies between modules | 9.2 Cyclic Include Detection |
| Multiple files | Deep function call chains exceeding depth of 7 | 9.3 Function Call Depth |

## SUMMARY OF MAJOR ISSUES

1. **Function Length Violations**: Multiple functions exceed 30-line limit
2. **File Length Violations**: Several files exceed 500-line limit (`screens.cpp`, `water.cpp`, `storage.cpp`, `pumps.cpp`)
3. **Global Variable Overuse**: Excessive global variables throughout project (exceeds limit of 3 in multiple files)
4. **Hardware Leakage**: Hardware pin references scattered across modules (`pumps.h`, `screens.h`, `display.h`, `auto_aqua.ino`)
5. **Include Hygiene**: Header includes exceed recommended limits in multiple files
6. **Architecture Smells**: "God Modules" and cyclic dependencies detected
7. **Magic Numbers**: Numerous numeric literals without `constexpr` definitions
8. **Delay Usage**: Blocking `delay()` calls instead of non-blocking timers
9. **Struct/Class Violations**: Multiple structs exceed field count limits (`Language` has 22 fields, `Configuration` has 8 fields)
10. **Macro Abuse**: Multiple macros throughout project exceed recommended limit of 20
11. **Public Method Overuse**: Classes expose too many public methods (`Pump` class has 9+ public methods)
12. **Mixed Responsibility**: Structs contain fields from different domains

This report identifies violations against the new coding guidelines across all project files. Each violation is documented with the specific guideline section reference for easy remediation.