// Simple pump toggle test
// Connect test outputs to the pump relays (or LED + resistor) for verification

#define INLET_PIN 38
#define OUTLET_PIN 39

void setup() {
  Serial.begin(9600);
  pinMode(INLET_PIN, OUTPUT);
  pinMode(OUTLET_PIN, OUTPUT);
  digitalWrite(INLET_PIN, LOW);
  digitalWrite(OUTLET_PIN, LOW);
  Serial.println("Pump toggle test started");
}

void loop() {
  Serial.println("Turning inlet ON");
  digitalWrite(INLET_PIN, HIGH);
  delay(1000);
  digitalWrite(INLET_PIN, LOW);
  Serial.println("Turning inlet OFF");
  delay(500);

  Serial.println("Turning outlet ON");
  digitalWrite(OUTLET_PIN, HIGH);
  delay(1000);
  digitalWrite(OUTLET_PIN, LOW);
  Serial.println("Turning outlet OFF");
  delay(1500);
}
