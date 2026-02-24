# CODING GUIDELINES STATUS REPORT

## Fixed in this pass

- Removed trailing whitespace across tracked source/header files in the root module scope.
- Replaced remaining C-style casts found in active code paths:
  - `screens_input.cpp`: `(unsigned long)val` → `static_cast<unsigned long>(val)`.
  - `screens_time.cpp`: `(uint32_t)(nowSecs % 86400ULL)` → `static_cast<uint32_t>(...)`.

## Remaining issues (high signal)

### 1) Line width violations (Guideline 1.3 ADVISORY)

The following files still contain lines over 120 chars and should be wrapped for readability:

- `language.h` (23 lines)
- `screens.h` (3 lines)
- `screens_config.cpp` (2 lines)
- `screens_input.cpp` (2 lines)
- `water_sensor.cpp` (2 lines)
- `chars.h` (1 line)
- `input.h` (1 line)
- `pumps.cpp` (1 line)
- `screens_pumps.cpp` (1 line)
- `screens_utils.cpp` (1 line)

### 2) Macro usage review (Guideline 5.3 STRONG)

Project still uses several preprocessor macros that should be reviewed for conversion to `constexpr` where practical (e.g., include-gated flags and legacy constants).

### 3) Structural complexity (Guidelines 1.x/2.x/7.x STRONG)

No automated complexity refactor was performed in this pass. Existing long declarations and dense UI/control flow should be split incrementally when those modules are actively modified.

## Notes

- This pass prioritized safe, mechanical cleanups (formatting + cast modernization) to avoid behavior changes.
- A deeper “entire project strict compliance” effort would require broader refactors across `language`, `screens`, and water-management modules.
