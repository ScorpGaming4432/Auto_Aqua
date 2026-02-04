#include <EEPROM.h>

const int ADDR = 10;

void setup() {
  Serial.begin(9600);
  Serial.println("EEPROM test starting");

  // write a test value
  uint16_t val = 0x1234;
  EEPROM.write(ADDR, (val >> 8) & 0xFF);
  EEPROM.write(ADDR + 1, val & 0xFF);
  Serial.print("Wrote 0x"); Serial.println(val, HEX);

  // read it back
  uint16_t r = ((uint16_t)EEPROM.read(ADDR) << 8) | EEPROM.read(ADDR + 1);
  Serial.print("Read 0x"); Serial.println(r, HEX);
}

void loop() {
  // nothing to do
  delay(1000);
}
