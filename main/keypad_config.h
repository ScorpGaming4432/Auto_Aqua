#ifndef KEYPADCONF_H
#define KEYPADCONF_H
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

const char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

const byte colPins[COLS] = { 44, 46, 48, 50 };
const byte rowPins[ROWS] = { 45, 47, 49, 51 };

extern Keypad keypad;

#endif