/**
 * D1 — 128×64 OLED Display (I2C / SSD1306)
 * ==========================================
 * Board : AI Thinker ESP32-CAM
 * Libs  : Adafruit SSD1306 + Adafruit GFX Library
 *
 * Wiring (I2C):
 *   OLED VCC → 3.3V   OLED GND → GND
 *   OLED SCL → GPIO 14   OLED SDA → GPIO 15
 *   Default address: 0x3C
 *
 * This example shows a live sensor dashboard on the OLED.
 * Requires DHT11 on GPIO 13 to show real temperature data.
 */
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_W   128
#define SCREEN_H    64
#define OLED_ADDR 0x3C
#define I2C_SDA    15
#define I2C_SCL    14
#define DHTPIN     13
#define DHTTYPE    DHT11

Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);
DHT dht(DHTPIN, DHTTYPE);

void drawDashboard(float tempC, float hum) {
  display.clearDisplay();

  // Header bar
  display.fillRect(0, 0, SCREEN_W, 12, WHITE);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.setCursor(18, 2);
  display.print("Smart IoT & AI Kit");

  // Temperature
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print("Temp:");
  display.setTextSize(2);
  display.setCursor(40, 14);
  display.printf("%.1fC", tempC);

  // Humidity
  display.setTextSize(1);
  display.setCursor(0, 38);
  display.print("Humidity:");
  display.setTextSize(2);
  display.setCursor(60, 36);
  display.printf("%.0f%%", hum);

  // Bottom status
  display.setTextSize(1);
  display.setCursor(0, 56);
  display.printf("Uptime: %lus", millis() / 1000);

  display.display();
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("[ERROR] OLED not found! Check wiring.");
    while (1) delay(500);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(10, 28);
  display.print("Initialising...");
  display.display();
  delay(2000);
  Serial.println("=== OLED Display Demo ===");
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (!isnan(t) && !isnan(h)) {
    drawDashboard(t, h);
    Serial.printf("Temp: %.1f°C  Hum: %.1f%%\n", t, h);
  } else {
    Serial.println("[WARN] DHT read failed — showing placeholder");
    drawDashboard(0, 0);
  }
  delay(2000);
}
