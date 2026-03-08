/**
 * S9 — NTC Thermistor Temperature Sensor
 * ========================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring (voltage divider):
 *   3.3V → [10kΩ series resistor] → GPIO 34 → [NTC 10k] → GND
 *
 * Typical NTC 10k specs: Nominal 10kΩ @ 25°C, B-coeff 3950.
 * Check your thermistor datasheet for exact B value.
 */
#define NTC_PIN     34
#define SERIES_R    10000.0f
#define NOMINAL_R   10000.0f
#define NOMINAL_T   25.0f
#define B_COEFF     3950.0f

float readTempC() {
  long s = 0;
  for (int i = 0; i < 10; i++) { s += analogRead(NTC_PIN); delay(5); }
  float r = SERIES_R * (4095.0f / (float)s * 10 - 1.0f);
  float t = log(r / NOMINAL_R) / B_COEFF + 1.0f / (NOMINAL_T + 273.15f);
  return 1.0f / t - 273.15f;
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== NTC Thermistor ===");
}

void loop() {
  float c = readTempC();
  Serial.printf("Temperature: %.2f°C  /  %.2f°F\n", c, c * 9 / 5 + 32);
  delay(1000);
}
