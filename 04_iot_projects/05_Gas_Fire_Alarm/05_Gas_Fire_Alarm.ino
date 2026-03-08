/**
 * P5 — Gas Leak & Fire Detection Alarm System
 * =============================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Gas Sensor A0 → GPIO 34   Gas Sensor D0 → GPIO 13
 *   Flame Sensor D0 → GPIO 12
 *   Buzzer S → GPIO 15   Red LED → GPIO 4 (flash LED)
 *
 * ⚠️ SAFETY DISCLAIMER:
 *   This is for EDUCATIONAL PURPOSES ONLY.
 *   Do NOT rely on this as your only safety device.
 *   Always install certified smoke/gas detectors.
 *
 * Features:
 *   - Dual detection: gas level + flame IR
 *   - 3-stage alert: CLEAN / WARNING / DANGER
 *   - Buzzer tone pattern changes with severity
 *   - Uptime and alert log via Serial
 */
#define GAS_A_PIN    34
#define GAS_D_PIN    13
#define FLAME_D_PIN  12
#define BUZZER_PIN   15
#define LED_PIN       4

#define GAS_WARNING  1000
#define GAS_DANGER   1800
#define PREHEAT_MS   120000UL

enum AlertLevel { CLEAN, WARNING, DANGER };
AlertLevel lastAlert = CLEAN;
uint32_t   alertCount = 0;

AlertLevel evaluateAlert(int gas, bool flame) {
  if (flame || gas > GAS_DANGER)  return DANGER;
  if (gas > GAS_WARNING)          return WARNING;
  return CLEAN;
}

void soundAlert(AlertLevel level) {
  switch (level) {
    case DANGER:
      for (int i = 0; i < 5; i++) {
        digitalWrite(BUZZER_PIN, HIGH); digitalWrite(LED_PIN, HIGH); delay(100);
        digitalWrite(BUZZER_PIN, LOW);  digitalWrite(LED_PIN, LOW);  delay(100);
      }
      break;
    case WARNING:
      digitalWrite(BUZZER_PIN, HIGH); digitalWrite(LED_PIN, HIGH); delay(500);
      digitalWrite(BUZZER_PIN, LOW);  digitalWrite(LED_PIN, LOW);  delay(500);
      break;
    case CLEAN:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(GAS_D_PIN,   INPUT);
  pinMode(FLAME_D_PIN, INPUT);
  pinMode(BUZZER_PIN,  OUTPUT);
  pinMode(LED_PIN,     OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN,    LOW);

  Serial.println("=== Gas & Fire Alarm System ===");
  Serial.printf("Sensor preheat: %lu seconds...\n", PREHEAT_MS / 1000);
  delay(PREHEAT_MS);
  Serial.println("Ready. Monitoring...");
}

void loop() {
  int  gas   = analogRead(GAS_A_PIN);
  bool flame = (digitalRead(FLAME_D_PIN) == LOW);
  bool gasD0 = (digitalRead(GAS_D_PIN)   == LOW);

  AlertLevel level = evaluateAlert(gas, flame || gasD0);

  const char* levelStr = (level == DANGER) ? "DANGER 🚨" :
                         (level == WARNING) ? "WARNING ⚠️" : "CLEAN ✅";

  Serial.printf("[%lus] Gas:%4d  Flame:%s  D0:%s  → %s\n",
                millis() / 1000, gas,
                flame ? "YES" : " NO",
                gasD0 ? "YES" : " NO",
                levelStr);

  if (level > CLEAN) {
    alertCount++;
    Serial.printf("!!! ALERT #%lu — %s !!!\n", alertCount, levelStr);
  }

  soundAlert(level);

  // Only delay when clean — during alert, loop fast
  if (level == CLEAN) delay(1000);
}
