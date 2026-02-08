#ifndef DEBUG_H
#define DEBUG_H

#include <Arduino.h>

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

extern const char* LOCATION_STRINGS[];
extern const char* ERROR_STRINGS[];

void SerialPrint();
void debugPrint(Location loc, const char* message);

// Efficient recursive template for any number of arguments
// Base case
void SerialPrint();

// Recursive template - handles any number of arguments efficiently
template<typename T, typename... Args>
void SerialPrint(T first, Args... rest);

#endif
