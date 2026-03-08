/**
 * A2 — 28BYJ-48 Stepper Motor + ULN2003 Driver
 * ===============================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : Stepper (built-in Arduino library)
 *
 * Wiring (ULN2003 → ESP32-CAM):
 *   IN1 → GPIO 12   IN2 → GPIO 13
 *   IN3 → GPIO 14   IN4 → GPIO 15
 *   VCC → 5V        GND → GND
 *
 * 28BYJ-48: 64 steps/rev internal × 1/64 gear = 4096 steps/rev
 * Note: pass pins in order IN1,IN3,IN2,IN4 to Stepper for smooth half-step.
 */
#include <Stepper.h>

#define STEPS_PER_REV 2048   // effective steps per output shaft revolution
#define RPM           10

// Pin order: IN1, IN3, IN2, IN4 (cross-wired for correct half-stepping)
Stepper stepper(STEPS_PER_REV, 12, 14, 13, 15);

void setup() {
  Serial.begin(115200);
  stepper.setSpeed(RPM);
  Serial.println("=== 28BYJ-48 Stepper Motor ===");
}

void loop() {
  Serial.println("One revolution CW");
  stepper.step(STEPS_PER_REV);
  delay(500);

  Serial.println("One revolution CCW");
  stepper.step(-STEPS_PER_REV);
  delay(500);
}
