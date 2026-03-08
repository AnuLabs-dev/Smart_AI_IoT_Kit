/**
 * S1 — DHT11 Temperature & Humidity Sensor
 * ==========================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : DHT sensor library by Adafruit (install via Library Manager)
 *
 * Wiring:
 *   DHT11 VCC  → 3.3V
 *   DHT11 DATA → GPIO 13  (+10kΩ pull-up to 3.3V recommended)
 *   DHT11 GND  → GND
 */
#include <DHT.h>

#define DHTPIN   13
#define DHTTYPE  DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht.begin();
  Serial.println("=== DHT11 Temperature & Humidity ===");
}

void loop() {
  delay(2000);
  float h  = dht.readHumidity();
  float tC = dht.readTemperature();
  float tF = dht.readTemperature(true);

  if (isnan(h) || isnan(tC)) {
    Serial.println("[ERROR] Failed to read DHT11 — check wiring!");
    return;
  }

  float hi = dht.computeHeatIndex(tC, h, false);
  Serial.printf("Temp: %.1f°C / %.1f°F  |  Humidity: %.1f%%  |  Heat Index: %.1f°C\n",
                tC, tF, h, hi);
}
