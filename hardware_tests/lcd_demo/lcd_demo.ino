#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Update address if your LCD uses a different one (0x27 common)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("AUTO AQUA TEST");
  lcd.setCursor(0,1);
  lcd.print("LCD Demo Running");
}

void loop() {
  // simple blink of message
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LCD Demo - ");
  lcd.print(millis()/1000);
  lcd.setCursor(0,1);
  lcd.print("seconds");
}
