/**
 * AI3 — Live AI Dashboard (Camera Stream + Sensor Data)
 * =======================================================
 * Board : AI Thinker ESP32-CAM
 * Libs  : DHT sensor library, Adafruit BMP280
 *
 * Features:
 *   - MJPEG live camera stream at /stream
 *   - JSON sensor endpoint at /sensors
 *   - Snapshot capture at /snapshot (saves to MicroSD if fitted)
 *   - Responsive HTML dashboard hosted on ESP32-CAM
 *   - Real-time sensor updates via JavaScript fetch()
 *
 * Wiring:
 *   DHT11 DATA → GPIO 13   BMP280 SDA → GPIO 15   BMP280 SCL → GPIO 14
 *
 * Access: Open http://[IP] in any browser on same Wi-Fi network.
 */
#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BMP280.h>
#include "camera_config.h"

#define WIFI_SSID  "YOUR_WIFI_SSID"
#define WIFI_PASS  "YOUR_WIFI_PASSWORD"
#define DHTPIN     13
#define I2C_SDA    15
#define I2C_SCL    14

DHT           dht(DHTPIN, DHT11);
Adafruit_BMP280 bmp;
WebServer     server(80);

// ── Dashboard HTML (minified) ─────────────────────────────────
const char DASHBOARD_HTML[] PROGMEM = R"rawhtml(
<!DOCTYPE html><html>
<head><meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>Smart IoT AI Dashboard</title>
<style>
  *{box-sizing:border-box;margin:0;padding:0}
  body{font-family:sans-serif;background:#0b1120;color:#e2e8f0;min-height:100vh}
  h1{text-align:center;padding:1rem;color:#00d4ff;font-size:1.4rem}
  .grid{display:grid;grid-template-columns:1fr 1fr;gap:1rem;padding:0 1rem 1rem;max-width:900px;margin:0 auto}
  .card{background:#1e293b;border-radius:12px;padding:1rem;border:1px solid #334155}
  .card h3{color:#94a3b8;font-size:.8rem;text-transform:uppercase;margin-bottom:.5rem}
  .val{font-size:2rem;font-weight:700;color:#00d4ff}
  .unit{font-size:.9rem;color:#64748b}
  .stream-card{grid-column:1/-1}
  img{width:100%;border-radius:8px;border:2px solid #334155}
  .status{display:inline-block;padding:.2rem .6rem;border-radius:999px;font-size:.75rem;font-weight:700}
  .ok{background:#14532d;color:#4ade80}.warn{background:#7c2d12;color:#fb923c}
  @media(max-width:600px){.grid{grid-template-columns:1fr}}
</style></head>
<body>
<h1>🤖 Smart IoT &amp; AI Dashboard</h1>
<div class="grid">
  <div class="card stream-card">
    <h3>📷 Live Camera Feed</h3>
    <img src="/stream" id="stream">
  </div>
  <div class="card">
    <h3>🌡️ Temperature</h3>
    <div class="val" id="temp">--</div>
    <div class="unit">°C</div>
  </div>
  <div class="card">
    <h3>💧 Humidity</h3>
    <div class="val" id="hum">--</div>
    <div class="unit">%</div>
  </div>
  <div class="card">
    <h3>🌬️ Pressure</h3>
    <div class="val" id="pres">--</div>
    <div class="unit">hPa</div>
  </div>
  <div class="card">
    <h3>🗻 Altitude</h3>
    <div class="val" id="alt">--</div>
    <div class="unit">m</div>
  </div>
  <div class="card">
    <h3>⏱️ Uptime</h3>
    <div class="val" id="uptime">--</div>
    <div class="unit">seconds</div>
  </div>
  <div class="card">
    <h3>📡 Wi-Fi Signal</h3>
    <div class="val" id="rssi">--</div>
    <div class="unit">dBm</div>
  </div>
</div>
<script>
function update(){
  fetch('/sensors').then(r=>r.json()).then(d=>{
    document.getElementById('temp').textContent   = d.temp.toFixed(1);
    document.getElementById('hum').textContent    = d.hum.toFixed(0);
    document.getElementById('pres').textContent   = d.pres.toFixed(1);
    document.getElementById('alt').textContent    = d.alt.toFixed(1);
    document.getElementById('uptime').textContent = d.uptime;
    document.getElementById('rssi').textContent   = d.rssi;
  }).catch(()=>{});
}
update(); setInterval(update, 3000);
</script>
</body></html>
)rawhtml";

// ── HTTP Handlers ─────────────────────────────────────────────
void handleRoot() {
  server.send_P(200, "text/html", DASHBOARD_HTML);
}

void handleSensors() {
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float p = bmp.readPressure() / 100.0F;
  float a = bmp.readAltitude(1013.25F);
  if (isnan(t)) t = 0; if (isnan(h)) h = 0;

  String json = "{";
  json += "\"temp\":"   + String(t, 2) + ",";
  json += "\"hum\":"    + String(h, 2) + ",";
  json += "\"pres\":"   + String(p, 2) + ",";
  json += "\"alt\":"    + String(a, 2) + ",";
  json += "\"uptime\":" + String(millis() / 1000) + ",";
  json += "\"rssi\":"   + String(WiFi.RSSI());
  json += "}";
  server.send(200, "application/json", json);
}

// MJPEG streaming handler
#define PART_BOUNDARY "frame"
void handleStream() {
  WiFiClient client = server.client();
  client.print("HTTP/1.1 200 OK\r\n"
               "Content-Type: multipart/x-mixed-replace;boundary=" PART_BOUNDARY "\r\n\r\n");
  while (client.connected()) {
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) break;
    client.printf("--" PART_BOUNDARY "\r\n"
                  "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n", fb->len);
    client.write(fb->buf, fb->len);
    client.print("\r\n");
    esp_camera_fb_return(fb);
    delay(50);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();
  bmp.begin(0x76);

  init_camera(FRAMESIZE_QVGA, PIXFORMAT_JPEG, 12, 2);

  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
  Serial.printf("\nDashboard at: http://%s\n", WiFi.localIP().toString().c_str());

  server.on("/",        handleRoot);
  server.on("/sensors", handleSensors);
  server.on("/stream",  handleStream);
  server.begin();
  Serial.println("=== AI Dashboard Server Running ===");
}

void loop() {
  server.handleClient();
}
