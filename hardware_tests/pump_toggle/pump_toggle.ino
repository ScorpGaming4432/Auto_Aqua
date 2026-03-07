// Simple pump toggle test
// Connect test outputs to the pump relays (or LED + resistor) for verification

constexpr uint8_t TESTING_PINS[] = {2, 3, 4, 5, 6, 7};

void setup() {
  Serial.begin(9600);
  for (uint8_t i = 0; i < sizeof(TESTING_PINS)/sizeof(TESTING_PINS[0]); i++) {
    pinMode(TESTING_PINS[i], OUTPUT);
  }
  for (uint8_t i = 0; i < sizeof(TESTING_PINS)/sizeof(TESTING_PINS[0]); i++) {
    digitalWrite(TESTING_PINS[i], LOW);
  }
  Serial.println("Pump toggle test started");
}

void loop() {
  Serial.println("Turning pumps ON");
  for (uint8_t i = 0; i < sizeof(TESTING_PINS)/sizeof(TESTING_PINS[0]); i++) {
    digitalWrite(TESTING_PINS[i], LOW);
  }
  delay(1000);
  for (uint8_t i = 0; i < sizeof(TESTING_PINS)/sizeof(TESTING_PINS[0]); i++) {
    digitalWrite(TESTING_PINS[i], HIGH);
  }
  Serial.println("Turning pumps OFF");
  delay(500);
}
