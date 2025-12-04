#include "display.h"
#include "display.h"
#include "chars.h"
#include "language.h"

#include <Arduino.h>
#include <Wire.h>
#include <string.h>

LiquidCrystal_I2C* lcd = nullptr;
uint32_t dimTimer = 0;
const byte LCD_ADDRESSES[2] = { 0x27, 0x3F };

bool findLCD() {
  Serial.println("Scanning I2C bus...");

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      for (byte i = 0; i < sizeof(LCD_ADDRESSES) / sizeof(LCD_ADDRESSES[0]); i++) {
        if (address == LCD_ADDRESSES[i]) {
          Serial.print("LCD found at 0x");
          Serial.println(address, HEX);

          lcd = new LiquidCrystal_I2C(address, 16, 2);
          lcd->init();
          lcd->backlight();

          return true;
        }
      }
    }
  }

  Serial.println("LCD not found!");
  return false;
}

void updateBacklight() {
  // `dimTimer` is updated by the main loop when a key is pressed.
  if (!lcd) return;
  lcd->setBacklight((millis() - dimTimer) <= 10000);
}

