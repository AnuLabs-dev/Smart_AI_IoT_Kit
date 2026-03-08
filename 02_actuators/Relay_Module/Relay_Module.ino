/**
 * A3 — 5V Relay Module
 * =====================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Relay VCC → 5V   Relay GND → GND   Relay IN → GPIO 12
 *   Relay COM → load common   Relay NO → load (normally open)
 *
 * SAFETY: When switching mains AC:
 *   - Never touch live terminals while powered.
 *   - Keep low-voltage (ESP32-CAM) side physically separated.
 *   - Relay supports up to 10A / 250VAC.
 *   - This module is ACTIVE LOW: LOW = relay ON, HIGH = relay OFF.
 */
#define RELAY_PIN 12

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);  // start OFF (active LOW)
  Serial.println("=== Relay Module ===");
}

void loop() {
  Serial.println("Relay ON");
  digitalWrite(RELAY_PIN, LOW);
  delay(2000);

  Serial.println("Relay OFF");
  digitalWrite(RELAY_PIN, HIGH);
  delay(2000);
}
