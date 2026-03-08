/**
 * S3 — HC-SR04 Ultrasonic Distance Sensor
 * =========================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   HC-SR04 VCC  → 5V
 *   HC-SR04 TRIG → GPIO 12
 *   HC-SR04 ECHO → GPIO 13  (use 1kΩ + 2kΩ voltage divider — ECHO is 5V!)
 *   HC-SR04 GND  → GND
 *
 *   Voltage divider: ECHO → 1kΩ → GPIO13
 *                                ↓ 2kΩ → GND
 */
#define TRIG_PIN 12
#define ECHO_PIN 13

float measureDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long dur = pulseIn(ECHO_PIN, HIGH, 30000UL);
  if (dur == 0) return -1;
  return dur * 0.034f / 2.0f;
}

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.println("=== HC-SR04 Ultrasonic ===");
}

void loop() {
  float d = measureDistanceCM();
  if (d < 0 || d > 400) Serial.println("OUT OF RANGE");
  else Serial.printf("Distance: %.1f cm (%.2f m)\n", d, d / 100.0f);
  delay(500);
}
