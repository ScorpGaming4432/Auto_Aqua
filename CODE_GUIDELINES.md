# Coding Guidelines

This document classifies every rule with one of three enforcement tags:

- **BLOCKER**: Safety/debug-critical violations that must be fixed before merge.
- **STRONG**: High-value engineering rules; deviations should be rare and justified.
- **ADVISORY**: Preferred readability/maintainability heuristics; use judgment.

---

## Exception Procedure (Waivers)

When a rule cannot reasonably be followed, add an inline waiver comment at the violation site.
Use this exact prefix for all waivers so code search stays reliable:

```cpp
// GUIDELINE-WAIVER: <rule-id> <reason>
```

Optional revisit metadata may be appended when useful (same prefix):

```cpp
// GUIDELINE-WAIVER: <rule-id> <reason> | revisit: <date|milestone>
```

Waivers are expected to be narrow, local, and temporary where possible.
Avoid one-off markers (for example, custom "raw memory" tags); always use `GUIDELINE-WAIVER`.

### Waiving line/function size limits in embedded code

For deeply timing-constrained interrupt paths, register maps, generated protocol shims, or
vendor-mandated callback signatures, line/function size limits may be waived when splitting the
code would reduce clarity, violate ordering constraints, or make debugging materially harder.
Document the constraint and, when possible, include a revisit milestone after hardware/SDK updates.

---

# SECTION 1 — Structural Size Rules

## 1.1 Function Length — STRONG

Limit functions to 30 lines. Functions must not exceed 40 lines.

## 1.2 Function Nesting Depth — STRONG

Limit function nesting depth to 3. Nesting depth must not exceed 4.

## 1.3 Line Width — ADVISORY

Limit lines to 100 characters. Lines should not exceed 120 characters.

## 1.4 File Length — STRONG

Limit files to 500 lines. Files must not exceed 800 lines.

---

# SECTION 2 — Function Discipline

## 2.1 Parameter Count — STRONG

Limit function parameters to 4. Functions must not exceed 5 parameters. Consider passing a struct or redesigning the interface.

## 2.2 Boolean Parameter Rule — STRONG

Avoid functions with two or more boolean parameters to ensure readable intent (e.g., `doStuff(true, false)` is unclear).

## 2.3 Multiple Responsibility Heuristic — STRONG

Functions should perform a single responsibility. Avoid combining more than one of the following: I/O calls, logic branching, string formatting, persistence calls, or hardware interaction.

Files should also focus on mainly one component, so that responsibility is divided equally and logically.

## 2.4 Early Return Enforcement — ADVISORY

Favor early returns (`if (!valid) return;`) over deeply nested `if` statements.

## 2.5 Function Contracts

Document a contract for every non-trivial function near its declaration or definition.

## 2.5.1 Required Contract Items

Include all of the following:

* Parameter units (e.g., `ms`, `%`, `liters`).
* Valid ranges.
* Sentinel behavior (`-1`, `0`, special enum values).
* Side effects (e.g., EEPROM writes, pump control).

## 2.5.2 Preconditions

Add explicit precondition checks for boundary-sensitive values before use.

Mandatory targets:

* Threshold ranges.
* Time fields.
* Pump indexes.

## 2.5.3 Postconditions and Invariants

For `AppState` updates and persistence flows, add postcondition/invariant comments that state what
must hold after the update.

## 2.5.4 Domain Examples

* `lowThreshold` / `highThreshold`: `%`, range `0-100`, sentinel `-1` as "unset", invariant
  `lowThreshold <= highThreshold`.
* `tankVolume`: liters, positive range, define whether `0` means "empty" or "unknown", and note
  EEPROM write side effects.
* Scheduling intervals: `ms`, bounded interval range, documented disable sentinel, invariant that
  interval pump actions do not overlap unsafely.

---

# SECTION 3 — Struct / Class Discipline

## 3.1 Field Count — STRONG

Limit struct fields to 10. Structs must not exceed 15 fields.

## 3.2 Mixed Responsibility Fields — STRONG

Avoid structs that contain fields from different domains (e.g., hardware-related, UI-related, and configuration names). Use namespaces or context clues.

## 3.3 Width Rule — ADVISORY

Struct declaration lines should not exceed 120 characters.

## 3.4 Public Data in Classes (C++) — STRONG

Limit public data members in classes to 3 or less to encourage encapsulation.

---

# SECTION 4 — Header Rules

## 4.1 Header Guard — BLOCKER

All headers must include `#pragma once` or traditional include guards.

## 4.2 Implementation in Header — STRONG

Do not define non-inline functions in headers. Exceptions: templates, `constexpr`, and `inline` functions.

## 4.3 Include Hygiene — BLOCKER

Limit header includes to 8 or less. Consider using forward declarations. Include cycles and accidental heavy transitive dependencies must be eliminated.

## 4.4 Using Namespace — STRONG

Do not use `using namespace` in header files.

---

# SECTION 5 — C++ Modern Discipline

## 5.1 Raw new/delete — BLOCKER

Avoid `new`, `delete`, `malloc`, or `free`. Use smart pointers. If manual memory management is essential (e.g., legacy code, special cases), add `// GUIDELINE-WAIVER: 5.1 <reason>` at the exact usage site.

## 5.2 Owning Raw Pointer — BLOCKER

Avoid owning raw pointer members in classes. Raw pointer members should be clearly non-owning (e.g., `* const` reference-like usage).

## 5.3 Macro Abuse — STRONG

Limit project macros to 20. Avoid macros that define non-constant expression logic.

## 5.4 C-Style Cast — STRONG

Use `static_cast`, `reinterpret_cast`, `const_cast`, or `dynamic_cast` instead of C-style casts (`(Type)value`).

Exception: low-level memory management. When needed, use `// GUIDELINE-WAIVER: 5.4 <reason>` at the cast site (optionally with `| revisit: <date|milestone>`).

---

# SECTION 6 — Arduino-Specific Rules

## 6.1 .ino Discipline — BLOCKER

`.ino` files must only contain includes, `setup()`, `loop()`, and delegation calls. Avoid logic loops, conditionals, or direct hardware calls within `.ino` files.

## 6.2 Global Variable Rule — STRONG

Limit global non-const variables to 3. Global non-const variables must not exceed 5.

## 6.3 Delay Usage — STRONG

Prefer non-blocking timers over `delay()` calls.

## 6.4 Hardware Leakage — BLOCKER

Avoid referencing hardware pin names outside dedicated hardware modules.

## 6.5 Helper Functions — ADVISORY

Helper functions may be introduced into the main `.ino`. Too many helper functions should be avoided and instead split into respective files.

## 6.6 LCD Magic Strings — STRONG

Avoid hardcoding LCD strings. Instead use a language struct or `constexpr` variables for all display strings.

---

# SECTION 7 — Complexity Metrics

## 7.1 Cyclomatic Complexity — STRONG

Limit cyclomatic complexity to 10. Complexity must not exceed 15. Count `if`, `else if`, `for`, `while`, `case`, `&&`, `||`.

## 7.2 Switch Case Length — STRONG

Extract `case` blocks exceeding 15 lines into separate functions.

## 7.3 Magic Numbers — STRONG

Define numeric literals (except 0, 1, -1, and powers of 2) as `constexpr` variables. Example: `constexpr int timeout_ms = 500;`.

---

# SECTION 8 — Formatting Enforcement

## 8.1 Indentation Consistency — STRONG

Do not mix tabs and spaces for indentation.

## 8.2 Trailing Whitespace — ADVISORY

Avoid trailing whitespace.

## 8.3 Multiple Blank Lines — ADVISORY

Limit consecutive blank lines to 2.

---

# SECTION 9 — Architecture Smell Detection

## 9.1 God Module — STRONG

Avoid "God Modules": files exceeding 800 lines that contain keywords from 3 or more distinct domains (e.g., ui, hardware, storage).

## 9.2 Cyclic Include Detection — BLOCKER

The include graph must not contain cycles.

## 9.3 Function Call Depth — ADVISORY

Limit function call chain depth to 7.

---

# Bonus: Personal "Discipline Mode"

## Strict Mode — ADVISORY

For stricter enforcement, apply the following limits:

- Function max 30 lines
- Max nesting 3
- Max struct fields 10
- Max parameters 4

---

# SECTION 10 — Enforcement & Rollout

## 10.1 Manual PR Checklist (High-Value Review Pass)

Use this checklist for every PR, even when no automated checks fail:

* [ ] Long functions are split or justified (`SECTION 1.1`, `SECTION 7.1`).
* [ ] Deep nesting is flattened with early returns or extracted helpers (`SECTION 1.2`, `SECTION 2.4`).
* [ ] Error-path logs include context (what failed + where + key inputs) (`SECTION 2.3`).
* [ ] Sentinel values are documented near declaration and use sites (`SECTION 7.3`).
* [ ] State mutations include a short intent comment when side effects are not obvious (`SECTION 2.3`, `SECTION 9.1`).

## 10.2 Lightweight Static-Analysis Hooks (Suggested)

These tools are optional but recommended because they have low setup overhead and can run locally or in CI:

* `clang-tidy` (`readability-function-size`, `readability-else-after-return`, `bugprone-branch-clone`)
  * Maps to: `SECTION 1.1`, `SECTION 1.2`, `SECTION 2.4`, `SECTION 7.1`.
* `cpplint` (line length, whitespace, include hygiene)
  * Maps to: `SECTION 1.3`, `SECTION 4.3`, `SECTION 8.1`, `SECTION 8.2`.
* `cppcheck --enable=warning,style,performance`
  * Maps to: `SECTION 2.3`, `SECTION 5.1`, `SECTION 5.2`, `SECTION 9.1`.
* `lizard` (function length / complexity trends)
  * Maps to: `SECTION 1.1`, `SECTION 1.2`, `SECTION 7.1`, `SECTION 7.2`.

Minimal hook pattern:

* pre-commit: run `cpplint` on changed files.
* pre-push or CI: run `clang-tidy`/`cppcheck` on touched modules and `lizard` on diffs.

## 10.3 Debug Readiness Checklist

Before merging, verify debug readiness on changed behavior:

* [ ] Failure paths expose a reproducible error code (or equivalent stable identifier).
* [ ] Logs/events include a meaningful Location tag (module + function or subsystem).
* [ ] Captured context is enough to replay the failure path (key inputs, state transition, and trigger).

## 10.4 Legacy Migration Note

For legacy cleanups, prioritize high-churn modules first to maximize risk reduction per change:

1. `screens_*.cpp`
2. `water_*`
3. `storage.cpp`

Apply the manual checklist in `10.1` when touching these files, even for small edits.

**Implement ONLY after everything seems to be intact**
