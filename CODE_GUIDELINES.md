# Coding Guidelines

---

# SECTION 1 — Structural Size Rules

## 1.1 Function Length

Limit functions to 30 lines. Functions must not exceed 40 lines.

## 1.2 Function Nesting Depth

Limit function nesting depth to 3. Nesting depth must not exceed 4.

## 1.3 Line Width

Limit lines to 100 characters. Lines must not exceed 120 characters.

## 1.4 File Length

Limit files to 500 lines. Files must not exceed 800 lines.

---

# SECTION 2 — Function Discipline

## 2.1 Parameter Count

Limit function parameters to 4. Functions must not exceed 5 parameters. Consider passing a struct or redesigning the interface.

## 2.2 Boolean Parameter Rule

Avoid functions with two or more boolean parameters to ensure readable intent (e.g., `doStuff(true, false)` is unclear).

## 2.3 Multiple Responsibility Heuristic

Functions should perform a single responsibility. Avoid combining more than one of the following: I/O calls, logic branching, string formatting, persistence calls, or hardware interaction.

Files should also focus on mainly one compoment, so that responsibility is divided equally and logically.

## 2.4 Early Return Enforcement

Favor early returns (`if (!valid) return;`) over deeply nested `if` statements.

---

# SECTION 3 — Struct / Class Discipline

## 3.1 Field Count

Limit struct fields to 10. Structs must not exceed 15 fields.

## 3.2 Mixed Responsibility Fields

Avoid structs that contain fields from different domains (e.g., hardware-related, UI-related, and configuration names). Use namespaces or context clues.

## 3.3 Width Rule

Struct declaration lines must not exceed 120 characters.

## 3.4 Public Data in Classes (C++)

Limit public data members in classes to 3 or less to encourage encapsulation.

---

# SECTION 4 — Header Rules

## 4.1 Header Guard

All headers must include `#pragma once` or traditional include guards.

## 4.2 Implementation in Header

Do not define non-inline functions in headers. Exceptions: templates, `constexpr`, and `inline` functions.

## 4.3 Include Hygiene

Limit header includes to 8 or less. Consider using forward declarations.

## 4.4 Using Namespace

Do not use `using namespace` in header files.

---

# SECTION 5 — C++ Modern Discipline

## 5.1 Raw new/delete

Avoid `new`, `delete`, `malloc`, or `free`. Use smart pointers. If manual memory management is essential (e.g., legacy code, special cases), mark with `// RAW MANUAL MEMORY`.

## 5.2 Owning Raw Pointer

Avoid owning raw pointer members in classes. Raw pointer members should be clearly non-owning (e.g., `* const` reference-like usage).

## 5.3 Macro Abuse

Limit project macros to 20. Avoid macros that define non-constant expression logic.

## 5.4 C-Style Cast

Use `static_cast`, `reinterpret_cast`, `const_cast`, or `dynamic_cast` instead of C-style casts (`(Type)value`).

EXCEPTION: Low-level memory management. Use a decorator comment `// C cast: <reason>`

---

# SECTION 6 — Arduino-Specific Rules

## 6.1 .ino Discipline

`.ino` files must only contain includes, `setup()`, `loop()`, and delegation calls. Avoid logic loops, conditionals, or direct hardware calls within `.ino` files.

## 6.2 Global Variable Rule

Limit global non-const variables to 3. Global non-const variables must not exceed 5.

## 6.3 Delay Usage

Prefer non-blocking timers over `delay()` calls.

## 6.4 Hardware Leakage

Avoid referencing hardware pin names outside dedicated hardware modules.

## 6.5 Helper functions

Helper functions may be introduced into the main `.ino`. Too many helper functions should be avoided and instead split into respective files.

## 6.6 LCD magic strings

Avoid hardcoding LCD strings. Instead use a language struct or `constexpr` variables for all display strings.

---

# SECTION 7 — Complexity Metrics

## 7.1 Cyclomatic Complexity

Limit cyclomatic complexity to 10. Complexity must not exceed 15. Count `if`, `else if`, `for`, `while`, `case`, `&&`, `||`.

## 7.2 Switch Case Length

Extract `case` blocks exceeding 15 lines into separate functions.

## 7.3 Magic Numbers

Define numeric literals (except 0, 1, -1, and powers of 2) as `constexpr` variables. Example: `constexpr int timeout_ms = 500;`.

---

# SECTION 8 — Formatting Enforcement

## 8.1 Indentation Consistency

Do not mix tabs and spaces for indentation.

## 8.2 Trailing Whitespace

Avoid trailing whitespace.

## 8.3 Multiple Blank Lines

Limit consecutive blank lines to 2.

---

# SECTION 9 — Architecture Smell Detection

## 9.1 God Module

Avoid "God Modules": files exceeding 800 lines that contain keywords from 3 or more distinct domains (e.g., ui, hardware, storage).

## 9.2 Cyclic Include Detection

The include graph must not contain cycles.

## 9.3 Function Call Depth

Limit function call chain depth to 7.

---

# SECTION 10 — Debuggability

## 10.1 Required Log Levels

Use exactly the following log levels and meanings:

* `ERROR`: operation failed, data is invalid, safety threshold violated, or any path that returns/propagates an `Errors` value.
* `WARN`: recoverable anomaly, fallback, retry, degraded mode, or clamped input that may affect behavior.
* `INFO`: lifecycle milestones and user-visible actions (mode/menu changes, start/stop, configuration loaded/saved).
* `TRACE`: high-frequency diagnostics for developers (sensor samples, loop deltas, branch tracing).

`TRACE` must be compile-time gated out of release builds (see 10.5).

## 10.2 Error-Path Logging Contract

Every error path must emit one log line before returning/propagating the error. That line must include all of the following fields:

* module/location tag (for example `Location::Sensor`, `Location::Menu`, or equivalent token),
* stable error identifier (`Errors::Xxx` enum or a stable error code constant),
* key runtime context required for diagnosis:
  * inputs,
  * current state,
  * threshold/limit values,
  * latest sensor snapshot (if applicable).

Do not log only free-form text for errors.

## 10.3 External Failure Mapping

Every externally visible failure (LCD text, serial user prompt, API response, UI status) must map to:

1. a user-safe message with no internal implementation details,
2. an internal debug identifier (`Errors::Xxx` or stable code) that can be traced in logs.

The user-safe message and debug identifier must be emitted together at the failure boundary.

## 10.4 State Transition Logging

Control loops and menu workflows must emit a one-line transition log on every state change:

* format: `<component> <from_state> -> <to_state> [trigger/context]`,
* level: `INFO` for normal transitions, `WARN` if transition is forced/recovery-based.

Do not split one transition across multiple log lines.

## 10.5 Compile-Time Log Gating

All logging must be controlled through compile-time flags. `DEBUG_SERIAL_ENABLED` is required as the primary gate for `SerialPrint` diagnostics.

Expected defaults:

* development/debug builds: `DEBUG_SERIAL_ENABLED=1` (`INFO`, `WARN`, `ERROR`, and optionally `TRACE` enabled),
* release builds: `DEBUG_SERIAL_ENABLED=0` (`ERROR` and minimal `WARN` only; no `TRACE`).

If additional flags are introduced (for example `TRACE_LOGS_ENABLED`), they must default to off in release builds.

## 10.6 Good vs Bad Debug Statement Examples

Good:

```cpp
SerialPrint::error(
  "[Location::Sensor] code=Errors::MoistureReadFailed "
  "input.pin=A0 state=Sampling threshold=300 sensor.raw=812"
);

SerialPrint::info(
  "[Location::Menu] Idle -> Watering trigger=BTN_OK selectedZone=2"
);
```

Bad:

```cpp
SerialPrint::error("read failed");
SerialPrint::println("something went wrong");
SerialPrint::info("state changed");
```

The bad examples are invalid because they omit `Location`, stable `Errors` code, and runtime context.

---

# Bonus: Personal "Discipline Mode"

## Strict Mode

For stricter enforcement, apply the following limits:

* Function max 30 lines
* Max nesting 3
* Max struct fields 10
* Max parameters 4

**Implement ONLY after everything seems to be intact**
