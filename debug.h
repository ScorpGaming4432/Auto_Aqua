#include <Arduino.h>

void SerialPrint() {
  Serial.println();  // Base case
}

// Handle Location + message pairs
template<typename First, typename... Rest>
void SerialPrint(Location loc, const char* message, Rest... rest) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.print(message);
  Serial.print(" ");
  SerialPrint(rest...);
}

// Fallback for standalone messages
template<typename First, typename... Rest>
void SerialPrint(First first, Rest... rest) {
  Serial.print(first);
  Serial.print(" ");
  SerialPrint(rest...);
}

void debugPrint(Location loc, const char* message) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  SerialPrint(message);
}

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

const char* LOCATION_STRINGS[] = {
  "[SETUP]", "[LOOP]", "[TANK]", "[AM]", "[THRESH]",
  "[PUMPS]", "[WATER]", "[CHARS]", "[TIME]", "[DUR]", "[STORAGE]",
  "[ERROR]", "[NOT_IMPLEMENTED]"
};

const char* ERROR_STRINGS[] = {
  "not set", "not set correctly", "Sensor Failure", "Pump Failure", "Unknown Error"
};

// Usage: SerialPrint(SETUP, "message", ERROR, "message2", "another message");


