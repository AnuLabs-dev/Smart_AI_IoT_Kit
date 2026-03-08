/**
 * S4 — PIR Motion Sensor (HC-SR501)
 * ===================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   PIR VCC → 5V   PIR OUT → GPIO 13   PIR GND → GND
 *
 * Adjustment:
 *   LEFT  pot = Sensitivity (3–7 m range)
 *   RIGHT pot = Hold time   (3–300 seconds)
 *   Allow 30–60 s warm-up before reliable detection.
 */
#define PIR_PIN   13
#define LED_FLASH  4   // onboard flash LED
#define WARMUP_MS 30000

unsigned long motionCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_FLASH, OUTPUT);
  digitalWrite(LED_FLASH, LOW);
  Serial.printf("Warming up PIR for %lu s...\n", WARMUP_MS / 1000);
  delay(WARMUP_MS);
  Serial.println("PIR ready — watching for motion.");
}

void loop() {
  if (digitalRead(PIR_PIN) == HIGH) {
    motionCount++;
    Serial.printf("[%lus] MOTION DETECTED! (count: %lu)\n",
                  millis() / 1000, motionCount);
    digitalWrite(LED_FLASH, HIGH); delay(200);
    digitalWrite(LED_FLASH, LOW);
  }
}
