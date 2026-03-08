/**
 * A5 — Active Buzzer & Tone Generation
 * ======================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Buzzer VCC → 3.3V   Buzzer GND → GND   Buzzer S → GPIO 15
 *
 * Active buzzer: HIGH = sound ON (fixed internal frequency).
 * For tone control, use a PASSIVE buzzer + ledcWriteTone() below.
 */
#define BUZZER_PIN 15
#define LEDC_CH    3

// Plays tone on passive buzzer (skip if using active buzzer)
void playTone(uint32_t freq, uint32_t durationMs) {
  ledcSetup(LEDC_CH, freq, 8);
  ledcAttachPin(BUZZER_PIN, LEDC_CH);
  ledcWrite(LEDC_CH, 128);
  delay(durationMs);
  ledcWrite(LEDC_CH, 0);
  delay(50);
}

// Simple beep for active buzzer
void beep(int times, int onMs = 150, int offMs = 100) {
  for (int i = 0; i < times; i++) {
    digitalWrite(BUZZER_PIN, HIGH); delay(onMs);
    digitalWrite(BUZZER_PIN, LOW);  delay(offMs);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER_PIN, OUTPUT);
  Serial.println("=== Buzzer Demo ===");
}

void loop() {
  // ── Active buzzer pattern ──────────────────────────
  Serial.println("Pattern: 3 short beeps");
  beep(3, 100, 100);
  delay(500);

  Serial.println("Pattern: 1 long beep");
  beep(1, 1000, 200);
  delay(500);

  // ── Passive buzzer scale (comment out beep calls above if using passive) ──
  // uint32_t notes[] = {262,294,330,349,392,440,494,523}; // C4-C5
  // for (auto n : notes) { playTone(n, 300); }

  delay(1500);
}
