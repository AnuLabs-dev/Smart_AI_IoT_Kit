/**
 * S8 — Flame Sensor (IR Flame Detector)
 * =======================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Sensor VCC → 3.3V   Sensor GND → GND
 *   Sensor A0  → GPIO 34   Sensor D0 → GPIO 13
 *   D0 threshold set by onboard potentiometer.
 */
#define FLAME_A  34
#define FLAME_D  13
#define BUZZER   15

void setup() {
  Serial.begin(115200);
  pinMode(FLAME_D, INPUT);
  pinMode(BUZZER, OUTPUT);
  Serial.println("=== Flame Sensor ===");
}

void loop() {
  bool fire = (digitalRead(FLAME_D) == LOW);
  int  pct  = constrain(map(analogRead(FLAME_A), 4095, 0, 0, 100), 0, 100);
  Serial.printf("IR Intensity: %3d%%  |  Flame: %s\n",
                pct, fire ? "DETECTED!" : "none");
  if (fire) {
    for (int i = 0; i < 5; i++) {
      digitalWrite(BUZZER, HIGH); delay(100);
      digitalWrite(BUZZER, LOW);  delay(100);
    }
  }
  delay(500);
}
