/**
 * AI8 — Smart Intruder Alert (Capstone Project)
 * ===============================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : Your Edge Impulse FOMO Arduino library (person detection model)
 *
 * Full pipeline:
 *   1. PIR sensor detects motion → wakes detection
 *   2. ESP32-CAM captures frame → runs FOMO inference
 *   3. If "person" detected with confidence > threshold:
 *        → Activate relay (siren / lock)
 *        → Sound buzzer pattern
 *        → Send Telegram push notification
 *        → Flash onboard LED
 *   4. Cooldown period before next detection
 *
 * BEFORE UPLOADING:
 *   1. Train FOMO model with "person" class on Edge Impulse.
 *   2. Deploy as Arduino Library → install.
 *   3. Replace library header below.
 *   4. Fill in Wi-Fi credentials and Telegram bot details.
 *
 * Telegram Bot Setup:
 *   1. Open Telegram → search @BotFather → /newbot → follow steps → copy token
 *   2. Message your new bot → visit:
 *      https://api.telegram.org/bot<TOKEN>/getUpdates
 *      Copy the "id" field from "chat" → that is your CHAT_ID
 *   3. Paste TOKEN and CHAT_ID into defines below
 *
 * Wiring:
 *   PIR Sensor OUT → GPIO 13
 *   Relay IN       → GPIO 12   (siren / door lock)
 *   Buzzer S       → GPIO 15
 *   Flash LED      → GPIO 4    (built-in)
 */

// ── REPLACE with your Edge Impulse project library header ────
#include <YOUR_PROJECT_NAME_inferencing.h>
// ─────────────────────────────────────────────────────────────

#include "esp_camera.h"
#include "camera_config.h"
#include <WiFi.h>
#include <HTTPClient.h>

// ── CONFIGURE THESE ───────────────────────────────────────────
#define WIFI_SSID       "YOUR_WIFI_SSID"
#define WIFI_PASS       "YOUR_WIFI_PASSWORD"
#define TG_BOT_TOKEN    "YOUR_TELEGRAM_BOT_TOKEN"  // from @BotFather
#define TG_CHAT_ID      "YOUR_CHAT_ID"             // your Telegram user/chat ID
// ─────────────────────────────────────────────────────────────

#define PIR_PIN          13
#define RELAY_PIN        12
#define BUZZER_PIN       15
#define FLASH_PIN         4

#define PERSON_LABEL    "person"
#define DETECT_CONF     0.60f     // FOMO confidence threshold
#define COOLDOWN_MS     30000UL   // 30 s between alerts
#define RELAY_ON_SEC    5

static uint8_t* rgb_buf   = nullptr;
uint32_t        lastAlert  = 0;
uint32_t        alertCount = 0;

// ── FOMO feature extraction ───────────────────────────────────
int ei_camera_get_data(size_t offset, size_t length, float* out_ptr) {
  size_t pix = offset * 3;
  for (size_t i = 0; i < length; i++) {
    out_ptr[i] = (rgb_buf[pix] << 16) | (rgb_buf[pix+1] << 8) | rgb_buf[pix+2];
    pix += 3;
  }
  return 0;
}

// ── Telegram notification ─────────────────────────────────────
bool sendTelegramAlert(int count) {
  if (WiFi.status() != WL_CONNECTED) return false;
  HTTPClient http;
  String msg = "🚨 Intruder Alert!%0A"
               "Persons detected: " + String(count) + "%0A"
               "Time: " + String(millis() / 1000) + "s uptime%0A"
               "Alert #" + String(alertCount);
  String url = "https://api.telegram.org/bot" + String(TG_BOT_TOKEN) +
               "/sendMessage?chat_id=" + String(TG_CHAT_ID) +
               "&text=" + msg + "&parse_mode=HTML";
  http.begin(url);
  http.setTimeout(5000);
  int code = http.GET();
  http.end();
  return (code == 200);
}

// ── Run FOMO inference & count persons ────────────────────────
int detectPersons() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) return -1;

  bool ok = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, rgb_buf);
  esp_camera_fb_return(fb);
  if (!ok) return -1;

  signal_t signal;
  signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  signal.get_data     = &ei_camera_get_data;

  ei_impulse_result_t result;
  if (run_classifier(&signal, &result, false) != EI_IMPULSE_OK) return -1;

  int count = 0;
  for (auto& bb : result.bounding_boxes) {
    if (bb.value >= DETECT_CONF && strcmp(bb.label, PERSON_LABEL) == 0) {
      count++;
      Serial.printf("  Person bb: x=%d y=%d w=%d h=%d conf=%.2f\n",
                    bb.x, bb.y, bb.width, bb.height, bb.value);
    }
  }
  return count;
}

void triggerAlert(int count) {
  alertCount++;
  Serial.printf("\n🚨 ALERT #%lu: %d person(s) detected!\n", alertCount, count);

  // Flash + buzz pattern
  for (int i = 0; i < 5; i++) {
    digitalWrite(FLASH_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH); delay(200);
    digitalWrite(FLASH_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);  delay(150);
  }

  // Relay ON (siren / lock)
  digitalWrite(RELAY_PIN, LOW);

  // Send Telegram (non-blocking timeout via HTTPClient)
  Serial.print("Sending Telegram... ");
  Serial.println(sendTelegramAlert(count) ? "sent ✅" : "failed ❌");

  delay(RELAY_ON_SEC * 1000);
  digitalWrite(RELAY_PIN, HIGH);

  lastAlert = millis();
}

void connectWiFi() {
  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries++ < 20) {
    Serial.print("."); delay(500);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\nWi-Fi connected: %s\n", WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\nWi-Fi failed — alerts will be local only");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN,    INPUT);
  pinMode(RELAY_PIN,  OUTPUT); digitalWrite(RELAY_PIN,  HIGH);
  pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, LOW);
  pinMode(FLASH_PIN,  OUTPUT); digitalWrite(FLASH_PIN,  LOW);

  // Allocate RGB buffer in PSRAM if available
  size_t bufSize = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT * 3;
  rgb_buf = psramFound()
    ? (uint8_t*)heap_caps_malloc(bufSize, MALLOC_CAP_SPIRAM)
    : (uint8_t*)malloc(bufSize);
  if (!rgb_buf) { Serial.println("[ERROR] RGB buffer alloc failed"); while(1) delay(500); }

  init_camera(FRAMESIZE_96X96, PIXFORMAT_JPEG, 12, 1);
  connectWiFi();

  Serial.println("=== Smart Intruder Alert System ===");
  Serial.printf("PIR pin: %d  Confidence threshold: %.0f%%\n",
                PIR_PIN, DETECT_CONF * 100);
  Serial.printf("Cooldown: %lu s  Relay on: %d s\n",
                COOLDOWN_MS / 1000, RELAY_ON_SEC);
  Serial.println("Watching for motion...");
}

void loop() {
  // Only run inference after PIR trigger AND cooldown elapsed
  if (digitalRead(PIR_PIN) != HIGH) return;
  if (millis() - lastAlert < COOLDOWN_MS) return;

  Serial.printf("\n[%lus] PIR triggered → running AI inference...\n",
                millis() / 1000);
  digitalWrite(FLASH_PIN, HIGH); // brief flash to illuminate scene

  int persons = detectPersons();
  digitalWrite(FLASH_PIN, LOW);

  if (persons < 0) {
    Serial.println("Inference error — skipping");
  } else if (persons > 0) {
    triggerAlert(persons);
  } else {
    Serial.println("  No person confirmed — likely PIR false positive");
    lastAlert = millis() - COOLDOWN_MS + 5000UL; // short 5s reset
  }
}
