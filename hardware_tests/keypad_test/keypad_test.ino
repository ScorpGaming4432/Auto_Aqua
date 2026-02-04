#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
const char keys[ROWS][COLS] = { { '1', '2', '3', 'A' },
                                { '4', '5', '6', 'B' },
                                { '7', '8', '9', 'C' },
                                { '*', '0', '#', 'D' } };

const byte rowPins[ROWS] = {30, 32, 34, 36};
const byte colPins[COLS] = {31, 33, 35, 37};

Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  Serial.println("Keypad test started. Press keys...");
}

void loop() {
  char k = keypad.getKey();
  if (k) {
    Serial.print("Key: ");
    Serial.println(k);
  }
}
