/**
 * P2 — IoT Weather Station → ThingSpeak Cloud
 * =============================================
 * Board : AI Thinker ESP32-CAM
 * Libs  : DHT sensor library, Adafruit BMP280
 *
 * Sensors used:
 *   DHT11 → GPIO 13  (temperature + humidity)
 *   BMP280 SDA → GPIO 15, SCL → GPIO 14 (pressure + altitude)
 *
 * ThingSpeak Fields:
 *   Field 1 = Temperature (°C)    Field 2 = Humidity (%)
 *   Field 3 = Pressure (hPa)      Field 4 = Altitude (m)
 *
 * CONFIGURE: WIFI_SSID, WIFI_PASS, THINGSPEAK_API_KEY below.
 * ThingSpeak free tier: minimum 15 s between updates.
 */
#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>

#define WIFI_SSID          "YOUR_WIFI_SSID"
#define WIFI_PASS          "YOUR_WIFI_PASSWORD"
#define THINGSPEAK_API_KEY "YOUR_WRITE_API_KEY"   // ThingSpeak channel write key
#define UPDATE_INTERVAL_MS  20000UL                // 20 s (ThingSpeak min: 15 s)

#define DHTPIN   13
#define I2C_SDA  15
#define I2C_SCL  14

DHT           dht(DHTPIN, DHT11);
Adafruit_BMP280 bmp;
WiFiClient    client;

void connectWiFi() {
  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
  Serial.printf("\nConnected — IP: %s\n", WiFi.localIP().toString().c_str());
}

bool sendToThingSpeak(float t, float h, float p, float a) {
  if (!client.connect("api.thingspeak.com", 80)) {
    Serial.println("[ERROR] ThingSpeak connection failed");
    return false;
  }
  String body = "field1=" + String(t, 2) +
                "&field2=" + String(h, 2) +
                "&field3=" + String(p, 2) +
                "&field4=" + String(a, 2);
  String req = "POST /update HTTP/1.1\r\n"
               "Host: api.thingspeak.com\r\n"
               "Connection: close\r\n"
               "X-THINGSPEAKAPIKEY: " + String(THINGSPEAK_API_KEY) + "\r\n"
               "Content-Type: application/x-www-form-urlencoded\r\n"
               "Content-Length: " + String(body.length()) + "\r\n\r\n" + body;
  client.print(req);
  client.stop();
  return true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();
  if (!bmp.begin(0x76)) {
    Serial.println("[ERROR] BMP280 not found!");
    while (1) delay(500);
  }
  connectWiFi();
  Serial.println("=== Weather Station → ThingSpeak ===");
}

void loop() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float p = bmp.readPressure() / 100.0F;
  float a = bmp.readAltitude(1013.25F);

  if (isnan(t) || isnan(h)) {
    Serial.println("[WARN] DHT read failed, skipping update");
  } else {
    Serial.printf("T:%.1f°C H:%.1f%% P:%.1fhPa Alt:%.1fm  → ThingSpeak...\n",
                  t, h, p, a);
    if (sendToThingSpeak(t, h, p, a))
      Serial.println("Upload OK ✅");
  }

  // Reconnect WiFi if dropped
  if (WiFi.status() != WL_CONNECTED) connectWiFi();

  delay(UPDATE_INTERVAL_MS);
}
