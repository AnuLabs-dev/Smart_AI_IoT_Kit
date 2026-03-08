/**
 * S7 — MQ Gas Sensor (MQ-2 / MQ-135)
 * =====================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Sensor VCC → 5V   Sensor GND → GND
 *   Sensor A0  → GPIO 34   Sensor D0 → GPIO 13
 *
 * IMPORTANT: Allow 24–48 h preheat on FIRST use.
 *   Normal power-on preheat: ~2 minutes (PREHEAT_MS below).
 *   Adjust ALERT_THRESHOLD after measuring clean-air baseline.
 */
#define GAS_A          34
#define GAS_D          13
#define BUZZER_PIN     15
#define ALERT_THRESHOLD 1500
#define PREHEAT_MS     120000UL

void setup() {
  Serial.begin(115200);
  pinMode(GAS_D, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("=== Gas Sensor ===");
  Serial.printf("Preheating for %lu s...\n", PREHEAT_MS / 1000);
  delay(PREHEAT_MS);
  Serial.println("Ready. Monitoring gas levels.");
}

void loop() {
  int  val   = analogRead(GAS_A);
  bool alert = (val > ALERT_THRESHOLD) || (digitalRead(GAS_D) == LOW);
  const char* level = val < 800 ? "CLEAN" : val < ALERT_THRESHOLD ?
                      "LOW"  : val < 2500 ? "HIGH" : "DANGER";
  Serial.printf("ADC: %4d  |  %s  |  Alert: %s\n",
                val, level, alert ? "YES" : "NO");
  if (alert) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH); delay(200);
      digitalWrite(BUZZER_PIN, LOW);  delay(200);
    }
  }
  delay(1000);
}
