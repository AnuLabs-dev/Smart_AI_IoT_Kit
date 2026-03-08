/**
 * A1 — SG90 Servo Motor
 * ======================
 * Board : AI Thinker ESP32-CAM
 * Lib   : ESP32Servo (install via Library Manager)
 *
 * Wiring:
 *   Servo Red (VCC)    → 5V
 *   Servo Brown (GND)  → GND
 *   Servo Orange (SIG) → GPIO 12
 *
 * Range: 0°–180°. PWM: 50 Hz, 1 ms (0°) to 2 ms (180°).
 */
#include <ESP32Servo.h>

#define SERVO_PIN 12
Servo myServo;

void setup() {
  Serial.begin(115200);
  myServo.attach(SERVO_PIN, 500, 2500); // min/max pulse µs
  Serial.println("=== SG90 Servo Motor ===");
}

void loop() {
  // Sweep 0→180°
  Serial.println("Sweeping 0° → 180°");
  for (int pos = 0; pos <= 180; pos++) {
    myServo.write(pos);
    delay(15);
  }
  delay(500);
  // Sweep 180→0°
  Serial.println("Sweeping 180° → 0°");
  for (int pos = 180; pos >= 0; pos--) {
    myServo.write(pos);
    delay(15);
  }
  delay(500);
}
