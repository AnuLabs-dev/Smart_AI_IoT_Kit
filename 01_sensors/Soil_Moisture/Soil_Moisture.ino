/**
 * S5 — Soil Moisture Sensor
 * ==========================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Sensor VCC → 3.3V   Sensor GND → GND
 *   Sensor A0  → GPIO 34 (analog, input-only)
 *   Sensor D0  → GPIO 13 (digital)
 *
 * Calibration thresholds (adjust for your sensor & soil type):
 *   ADC > 2500 = Dry    |   1500–2500 = Moist   |   < 1500 = Wet
 */
#define SOIL_A 34
#define SOIL_D 13
#define DRY_THRESH   2500
#define MOIST_THRESH 1500

int smoothRead(int pin, int n = 10) {
  long s = 0;
  for (int i = 0; i < n; i++) { s += analogRead(pin); delay(5); }
  return s / n;
}

void setup() {
  Serial.begin(115200);
  pinMode(SOIL_D, INPUT);
  Serial.println("=== Soil Moisture Sensor ===");
}

void loop() {
  int raw     = smoothRead(SOIL_A);
  int pct     = constrain(map(raw, 4095, 0, 0, 100), 0, 100);
  const char* status = raw > DRY_THRESH ? "DRY - water now!" :
                       raw > MOIST_THRESH ? "MOIST - OK" : "WET - no watering needed";
  Serial.printf("ADC: %4d  |  Moisture: %3d%%  |  %s\n", raw, pct, status);
  delay(2000);
}
