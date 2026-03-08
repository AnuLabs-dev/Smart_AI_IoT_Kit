/**
 * A6 — Potentiometer (Analog Input)
 * ===================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Pot left pin  → GND
 *   Pot middle    → GPIO 34 (wiper / analog out)
 *   Pot right pin → 3.3V
 *
 * Also controls servo position and LED brightness as demo.
 * GPIO 34 is input-only — do not try to drive it as output.
 */
#include <ESP32Servo.h>

#define POT_PIN    34
#define SERVO_PIN  12
#define LED_PIN    33   // onboard status LED (active LOW)
#define LED_CH     4

Servo myServo;

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN, 500, 2500);
  ledcSetup(LED_CH, 5000, 8);
  ledcAttachPin(LED_PIN, LED_CH);
  Serial.println("=== Potentiometer Demo ===");
  Serial.println("Rotate pot: controls servo angle & LED brightness");
}

void loop() {
  int raw    = analogRead(POT_PIN);                   // 0–4095
  int angle  = map(raw, 0, 4095, 0, 180);            // servo degrees
  int bright = map(raw, 0, 4095, 255, 0);            // LED (inverted active LOW)
  int pct    = map(raw, 0, 4095, 0, 100);

  myServo.write(angle);
  ledcWrite(LED_CH, bright);

  Serial.printf("Raw: %4d  |  %3d%%  |  Servo: %3d°  |  LED: %3d\n",
                raw, pct, angle, 255 - bright);
  delay(50);
}
