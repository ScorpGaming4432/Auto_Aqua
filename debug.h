#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

// Set to 0 to compile out debug serial output and related strings.
#ifndef DEBUG_SERIAL_ENABLED
#define DEBUG_SERIAL_ENABLED 1
#endif

enum Location {
  SETUP = 0,
  LOOP = 1,
  TANK = 2,
  AM = 3,
  THRESH = 4,
  PUMPS = 5,
  WATER = 6,
  CHARS = 7,
  TIME = 8,
  DUR = 9,
  STORAGE = 10
};

enum Errors {
  NOT_SET = 0,
  NOT_CORRECT = 1,
  SENSOR_FAILURE = 2,
  PUMP_FAILURE = 3,
  UNKNOWN_ERROR = 4
};

#if DEBUG_SERIAL_ENABLED
inline void printLocationTag(Location tag) {
  switch (tag) {
    case SETUP:   Serial.print(F("SETUP")); break;
    case LOOP:    Serial.print(F("LOOP")); break;
    case TANK:    Serial.print(F("TANK")); break;
    case AM:      Serial.print(F("AM")); break;
    case THRESH:  Serial.print(F("THRESH")); break;
    case PUMPS:   Serial.print(F("PUMPS")); break;
    case WATER:   Serial.print(F("WATER")); break;
    case CHARS:   Serial.print(F("CHARS")); break;
    case TIME:    Serial.print(F("TIME")); break;
    case DUR:     Serial.print(F("DUR")); break;
    case STORAGE: Serial.print(F("STORAGE")); break;
    default:      Serial.print(F("UNKNOWN")); break;
  }
}

template<typename... Args>
inline void SerialPrint(Location tag, Args... args) {
  Serial.print('[');
  printLocationTag(tag);
  Serial.print(F("] "));
  (Serial.print(args), ...);
  Serial.println();
}
#else
template<typename... Args>
inline void SerialPrint(Location, Args...) {}
#endif

#endif
