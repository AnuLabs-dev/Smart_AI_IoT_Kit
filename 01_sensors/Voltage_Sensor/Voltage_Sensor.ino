/**
 * S10 — Voltage Sensor Module (0–25V)
 * =====================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Sensor +  → Input positive (battery / supply)
 *   Sensor -  → Input GND (shared with ESP32-CAM GND)
 *   Sensor S  → GPIO 34 (signal)
 *
 * Module uses 30kΩ/7.5kΩ divider → ratio = 5.0
 * Max safe input: 25 V
 * Adjust CALIBRATION_FACTOR after verifying with a multimeter.
 */
#define VOLT_PIN          34
#define ADC_REF           3.3f
#define ADC_RES           4096.0f
#define DIVIDER_RATIO     5.0f
#define CALIBRATION       1.00f

float readVoltage() {
  long s = 0;
  for (int i = 0; i < 20; i++) { s += analogRead(VOLT_PIN); delay(2); }
  return (s / 20.0f / ADC_RES) * ADC_REF * DIVIDER_RATIO * CALIBRATION;
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== Voltage Sensor (0–25V) ===");
}

void loop() {
  float v = readVoltage();
  Serial.printf("Voltage: %.2f V  [%s]\n", v,
                v < 6 ? "LOW" : v < 15 ? "OK" : "HIGH");
  delay(1000);
}
