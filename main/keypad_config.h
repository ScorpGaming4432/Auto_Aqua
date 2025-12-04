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

const byte colPins[COLS] = { 31, 33, 35, 37 };
const byte rowPins[ROWS] = { 30, 32, 34, 36 };

extern Keypad keypad;

#endif