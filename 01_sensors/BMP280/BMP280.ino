/**
 * S2 — BMP280 Barometric Pressure, Temperature & Altitude
 * =========================================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : Adafruit BMP280 Library + Adafruit Unified Sensor
 *
 * Wiring (I2C):
 *   BMP280 VCC → 3.3V   BMP280 GND → GND
 *   BMP280 SCL → GPIO 14   BMP280 SDA → GPIO 15
 *   Default I2C address: 0x76 (SDO→GND) or 0x77 (SDO→VCC)
 */
#include <Wire.h>
#include <Adafruit_BMP280.h>

#define I2C_SDA      15
#define I2C_SCL      14
#define BMP_ADDR     0x76
#define SEA_LEVEL_HPA 1013.25f

Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  Serial.println("=== BMP280 Pressure & Altitude ===");
  if (!bmp.begin(BMP_ADDR)) {
    Serial.println("[ERROR] BMP280 not found! Check wiring & address.");
    while (1) delay(500);
  }
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                  Adafruit_BMP280::SAMPLING_X2,
                  Adafruit_BMP280::SAMPLING_X16,
                  Adafruit_BMP280::FILTER_X16,
                  Adafruit_BMP280::STANDBY_MS_500);
  Serial.println("BMP280 ready.");
}

void loop() {
  Serial.printf("Temp: %.2f°C  |  Pressure: %.2f hPa  |  Altitude: %.2f m\n",
                bmp.readTemperature(),
                bmp.readPressure() / 100.0F,
                bmp.readAltitude(SEA_LEVEL_HPA));
  delay(2000);
}
