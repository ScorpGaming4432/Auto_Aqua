/**
 * ============================================================================
 * SCREENS.CPP - Main Screen Implementation
 * ============================================================================
 */

#include "screens.h"
#include "display.h"
#include "chars.h"
#include <Arduino.h>

/**
 * Display animated splash screen
 */
void splashScreen() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  uint8_t scratch[8];

  lcd.setCursor(3, 0);
  lcd.print("AUTO");
  lcd.setCursor(4, 1);
  lcd.print("AQUA");

  uint8_t slots[4] = { 0, 1, 2, 3 };

  for (uint8_t r = 0; r <= 8; r++) {
    lcd.setCursor(11, 0);
    lcd.write(0);
    lcd.setCursor(10, 1);
    lcd.write(1);
    lcd.write(2);
    lcd.write(3);

    Glyphs::animateIcon(slots, r, scratch);
    delay(80);
  }
  delay(1000);
}
