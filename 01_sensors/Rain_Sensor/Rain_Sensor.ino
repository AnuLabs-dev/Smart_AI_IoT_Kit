/**
 * S6 — Rain Sensor
 * =================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Sensor VCC → 3.3V   Sensor GND → GND
 *   Sensor A0  → GPIO 34 (analog)
 *   Sensor D0  → GPIO 13 (digital: LOW = rain detected)
 */
#define RAIN_A  34
#define RAIN_D  13

void setup() {
  Serial.begin(115200);
  pinMode(RAIN_D, INPUT);
  Serial.println("=== Rain Sensor ===");
}

void loop() {
  int  raw    = analogRead(RAIN_A);
  int  pct    = constrain(map(raw, 4095, 0, 0, 100), 0, 100);
  bool rain   = (digitalRead(RAIN_D) == LOW);
  const char* desc = pct < 20 ? "DRY" : pct < 50 ? "LIGHT RAIN" :
                     pct < 80 ? "MODERATE RAIN" : "HEAVY RAIN";
  Serial.printf("ADC: %4d  |  %3d%%  |  %-15s | D0: %s\n",
                raw, pct, desc, rain ? "RAIN" : "DRY");
  delay(1000);
}
