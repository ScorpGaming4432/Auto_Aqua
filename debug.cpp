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

void SerialPrint(const char* message) {
  Serial.println(message);
}

void SerialPrint(const char* message, int value) {
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(const char* message, unsigned int value) {
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(const char* message, long value) {
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(const char* message, unsigned long value) {
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(const char* message, const char* value) {
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(Location loc, const char* message) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.println(message);
}

void SerialPrint(Location loc, const char* message, int value) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(Location loc, const char* message, unsigned int value) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(Location loc, const char* message, long value) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(Location loc, const char* message, unsigned long value) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void SerialPrint(Location loc, const char* message, const char* value) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.print(message);
  Serial.print(" ");
  Serial.println(value);
}

void debugPrint(Location loc, const char* message) {
  Serial.print(LOCATION_STRINGS[loc]);
  Serial.print(": ");
  Serial.println(message);
}