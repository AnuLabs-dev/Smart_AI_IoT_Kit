/**
 * P4 — Smart Plant Monitor & Auto Watering
 * ==========================================
 * Board : AI Thinker ESP32-CAM
 * Libs  : Adafruit SSD1306, Adafruit GFX, DHT sensor library
 *
 * Wiring:
 *   Soil Sensor A0 → GPIO 34
 *   DHT11 DATA     → GPIO 13
 *   Relay (pump) IN→ GPIO 12   (connects small 5V submersible pump)
 *   OLED SDA       → GPIO 15   OLED SCL → GPIO 14
 *
 * Logic:
 *   - Reads soil moisture every 10 s.
 *   - If soil is DRY (ADC > DRY_THRESHOLD): pump on for PUMP_ON_SEC.
 *   - Waits COOLDOWN_SEC before next watering cycle.
 *   - OLED shows live readings.
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SOIL_PIN      34
#define RELAY_PIN     12
#define DHTPIN        13
#define I2C_SDA       15
#define I2C_SCL       14

#define DRY_THRESHOLD  2500    // ADC value — above = dry
#define PUMP_ON_SEC    5       // seconds to run pump
#define COOLDOWN_SEC   120     // seconds between watering cycles
#define READ_INTERVAL  10000UL // ms between sensor reads

Adafruit_SSD1306 oled(128, 64, &Wire, -1);
DHT dht(DHTPIN, DHT11);

uint32_t lastRead    = 0;
uint32_t lastWater   = 0;
bool     pumpActive  = false;
int      waterCount  = 0;

int smoothSoil(int samples = 10) {
  long s = 0;
  for (int i = 0; i < samples; i++) { s += analogRead(SOIL_PIN); delay(5); }
  return s / samples;
}

void updateOLED(int soil, int soilPct, float temp, float hum, bool watering) {
  oled.clearDisplay();
  oled.fillRect(0, 0, 128, 10, WHITE);
  oled.setTextColor(BLACK); oled.setTextSize(1);
  oled.setCursor(12, 1); oled.print("Plant Monitor  v1.0");

  oled.setTextColor(WHITE);
  oled.setCursor(0, 13); oled.printf("Soil:  %3d%%", soilPct);
  oled.setCursor(0, 25); oled.printf("Temp:  %.1fC", temp);
  oled.setCursor(0, 37); oled.printf("Humid: %.0f%%", hum);
  oled.setCursor(0, 49);
  if (watering) {
    oled.print("WATERING NOW 💧");
  } else {
    oled.printf("Waters: %d  Up:%lus", waterCount, millis()/1000);
  }
  oled.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // relay off (active LOW)

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("[WARN] OLED not found — continuing without display");
  } else {
    oled.clearDisplay(); oled.setTextSize(1); oled.setTextColor(WHITE);
    oled.setCursor(10, 28); oled.print("Plant Monitor"); oled.display();
    delay(2000);
  }
  Serial.println("=== Smart Plant Monitor ===");
}

void loop() {
  if (millis() - lastRead < READ_INTERVAL) return;
  lastRead = millis();

  int   soil    = smoothSoil();
  int   soilPct = constrain(map(soil, 4095, 0, 0, 100), 0, 100);
  float temp    = dht.readTemperature();
  float hum     = dht.readHumidity();

  if (isnan(temp)) temp = 0;
  if (isnan(hum))  hum  = 0;

  Serial.printf("Soil ADC: %d (%d%%)  Temp: %.1f°C  Hum: %.0f%%\n",
                soil, soilPct, temp, hum);

  bool needsWater = (soil > DRY_THRESHOLD) &&
                    (millis() - lastWater > (uint32_t)COOLDOWN_SEC * 1000);

  if (needsWater) {
    Serial.printf("Soil is DRY — watering for %d s...\n", PUMP_ON_SEC);
    updateOLED(soil, soilPct, temp, hum, true);
    digitalWrite(RELAY_PIN, LOW);   // pump ON
    delay(PUMP_ON_SEC * 1000);
    digitalWrite(RELAY_PIN, HIGH);  // pump OFF
    lastWater = millis();
    waterCount++;
    Serial.printf("Watering done. Total cycles: %d\n", waterCount);
  }

  updateOLED(soil, soilPct, temp, hum, false);
}
