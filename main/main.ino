#include "language.h"
#include "chars.h"
#include "keypad_config.h"
#include "display.h"
#include "screens.h"
#include "utils.h"

uint8_t languageIndex = 1;
uint32_t tankVolume = 0;
Language langbuf;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!findLCD()) {
    while (true)
      ;  // stop program
  }

  dimTimer = millis();
  splashScreen();
  delay(2000);
  languageIndex = langConfigScreen(languageIndex);
  // After language selection, load language-specific glyphs into CGRAM
  langbuf = readLanguage(languageIndex);
  loadLanguageGlyphs(languageIndex);
  
  tankVolume = tankVolumeScreen(langbuf.tankTitle);
}


void loop() {
  char key = keypad.getKey();

  if (key) {
    dimTimer = millis();
    Serial.print(key);
  }

  updateBacklight();
  delay(10);
}