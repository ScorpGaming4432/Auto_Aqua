#include "debug.h"

const char* LOCATION_STRINGS[] = {
  "SETUP", "LOOP", "TANK", "AM", "THRESH",
  "PUMPS", "WATER", "CHARS", "TIME", "DUR", "STORAGE",
  "ERROR", "NOT_IMPLEMENTED"
};

const char* ERROR_STRINGS[] = {
  "not set", "not set correctly", "Sensor Failure", "Pump Failure", "Unknown Error"
};

template<typename... Args>
void SerialPrint(Location tag, Args... args) {
  Serial.print("[");
  Serial.print(LOCATION_STRINGS[tag]);
  Serial.print("] ");

  (Serial.print(args), ...);
  Serial.println();
}
