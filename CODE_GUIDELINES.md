Below is an implementable rule set, grouped by category.

No fluff. Just violations.


---

🔴 SECTION 1 — Structural Size Rules

These are your average anti-chaos constraints.

1.1 Function Length

❌ ERROR if function > 40 lines

⚠ WARNING if > 30 lines


---

1.2 Function Nesting Depth

❌ ERROR if nesting depth > 4

⚠ WARNING if > 3


Track {} depth inside function.

Deep nesting = broken abstraction.


---

1.3 Line Width

❌ ERROR if line > 120 chars

⚠ WARNING if > 100 chars


Yes, your 600-character struct dies here.


---

1.4 File Length

⚠ WARNING if file > 500 lines

❌ ERROR if > 800 lines


Large files = multiple responsibilities.


---

🟠 SECTION 2 — Function Discipline

2.1 Parameter Count

❌ ERROR if > 5 parameters

⚠ WARNING if > 4


More than 5 means:

pass struct

redesign interface



---

2.2 Boolean Parameter Rule

❌ ERROR if function has 2+ bool parameters


Example of crime:

doStuff(true, false);

That’s unreadable intent.


---

2.3 Multiple Responsibility Heuristic

Flag if:

Function contains more than 1 of:

I/O calls

Logic branching

Formatting strings

Persistence calls

Hardware interaction



(Yes, heuristic-based. Good enough.)


---

2.4 Early Return Enforcement

⚠ WARNING if function has nested if that could be early-returned


Encourage:

if (!valid)
    return;

Instead of:

if (valid) {
    ...
}


---

🟡 SECTION 3 — Struct / Class Discipline

3.1 Field Count

❌ ERROR if struct has > 15 fields

⚠ WARNING if > 10



---

3.2 Mixed Responsibility Fields

Flag if struct contains:

hardware-related names (pin, port, baud) AND

UI names (screen, menu, cursor) AND

config names (timeout, mode, enabled)


That’s 3 domains in one blob.
Instead you can try using namespaces as often as you can. If that's not viable, use context clues.


---

3.3 Width Rule

❌ ERROR if any struct declaration line > 120 chars


Kill horizontal sprawl.


---

3.4 Public Data in Classes (C++)

⚠ WARNING if class has > 3 public data members


Encourage encapsulation.


---

🟢 SECTION 4 — Header Rules

4.1 Header Guard

❌ ERROR if header lacks:

#pragma once OR

traditional include guards




---

4.2 Implementation in Header

❌ ERROR if non-inline function defined in header


Except:

templates

constexpr

inline



---

4.3 Include Hygiene

⚠ WARNING if header includes more than 8 other headers


Suggest forward declarations.


---

4.4 Using Namespace

❌ ERROR if using namespace appears in header


Immediate execution.


---

🔵 SECTION 5 — C++ Modern Discipline

5.1 Raw new/delete

❌ ERROR on:

new

delete

malloc

free



Unless explicitly marked with comment:

// NOLINT_MANUAL_MEMORY


---

5.2 Owning Raw Pointer

⚠ WARNING if class contains raw pointer member
Unless clearly non-owning (heuristic: name contains * const reference-like usage).



---

5.3 Macro Abuse

❌ ERROR if project defines > 20 macros

⚠ WARNING if macro defines non-constant expression logic



---

5.4 C-Style Cast

❌ ERROR on (Type)value
Require:

static_cast

reinterpret_cast

etc.



---

🟣 SECTION 6 — Arduino-Specific Rules

For Arduino projects.

6.1 .ino Discipline

❌ ERROR if .ino contains:

logic loops

conditionals

hardware calls



Allow only:

includes

setup()

loop()

delegation calls



---

6.2 Global Variable Rule

❌ ERROR if global non-const variable count > 5

⚠ WARNING if > 3



---

6.3 Delay Usage

⚠ WARNING on delay() usage
Encourage non-blocking timers.



---

6.4 Hardware Leakage

⚠ WARNING if hardware pin names appear outside hardware module


If screens.cpp references PIN_LED, that’s architecture rot.


---

⚫ SECTION 7 — Complexity Metrics

Now we get serious.

7.1 Cyclomatic Complexity

❌ ERROR if > 15

⚠ WARNING if > 10


Count:

if

else if

for

while

case

&&

||



---

7.2 Switch Case Length

⚠ WARNING if single case > 15 lines


Extract to function.


---

7.3 Magic Numbers

⚠ WARNING on numeric literals except:

0

1

-1

powers of 2



Everything else should be:

constexpr int timeout_ms = 500;


---

⚪ SECTION 8 — Formatting Enforcement

8.1 Indentation Consistency

❌ ERROR if tabs and spaces mixed



---

8.2 Trailing Whitespace

❌ ERROR



---

8.3 Multiple Blank Lines

⚠ WARNING if > 2 consecutive blank lines



---

🧠 SECTION 9 — Architecture Smell Detection

Heuristic-based but useful.

9.1 God Module

❌ ERROR if file:

> 800 lines AND



contains 3+ domain keywords (ui, hardware, storage)




---

9.2 Cyclic Include Detection

❌ ERROR if include graph has cycle



---

9.3 Function Call Depth

⚠ WARNING if call chain depth > 7


Stack spaghetti detection.


---

🧨 Bonus: Personal “Discipline Mode”

Add optional mode:

Strict Mode

Function max 30 lines

Max nesting 3

Max struct fields 10

Max params 4


Flip a config flag.
