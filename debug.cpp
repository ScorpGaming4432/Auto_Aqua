#include "debug.h"

const char* LOCATION_STRINGS[] = {
  "[SETUP]", "[LOOP]", "[TANK]", "[AM]", "[THRESH]",
  "[PUMPS]", "[WATER]", "[CHARS]", "[TIME]", "[DUR]", "[STORAGE]",
  "[ERROR]", "[NOT_IMPLEMENTED]"
};

const char* ERROR_STRINGS[] = {
  "not set", "not set correctly", "Sensor Failure", "Pump Failure", "Unknown Error"
};

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

// Specialization for numeric types to avoid ambiguity
template<typename... Rest>
void SerialPrint(long long first, Rest... rest) {
  Serial.print((long)first);
  Serial.print(" ");
  SerialPrint(rest...);
}

void debugPrint(Location loc, const char* message) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.println(message);
}

// Explicit template instantiations
template void SerialPrint<>(Location, const char*);
template void SerialPrint<>(const char*, const char*);
template void SerialPrint<>(const char*, long long);
template void SerialPrint<>(long long, const char*);
template void SerialPrint<>(long long);