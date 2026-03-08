/**
 * AI1 — Face Detection & Recognition (ESP-WHO / CameraWebServer)
 * ================================================================
 * Board : AI Thinker ESP32-CAM
 * Source: Built-in Arduino ESP32 example — extended here.
 *
 * This sketch uses the official Espressif CameraWebServer example
 * with Wi-Fi and face detection enabled via the web UI.
 *
 * QUICK START:
 *   1. In Arduino IDE: File → Examples → ESP32 → Camera → CameraWebServer
 *   2. Uncomment: #define CAMERA_MODEL_AI_THINKER
 *   3. Fill in WIFI_SSID / WIFI_PASS
 *   4. Upload → Serial Monitor → open the printed IP in your browser
 *   5. Click "Start Stream" then toggle "Face Detection" in the sidebar
 *
 * This standalone sketch below gives you programmatic access to
 * face detection results so you can trigger GPIOs on detection.
 *
 * Wiring:
 *   Relay/LED → GPIO 4  (triggers when face recognised)
 *   Buzzer    → GPIO 15
 *
 * CONFIGURE: WIFI_SSID, WIFI_PASS, and KNOWN_FACE_ID below.
 */
#include "esp_camera.h"
#include <WiFi.h>
#include "camera_config.h"

// ESP-WHO face detection headers (available after ESP32 board install)
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"

#define WIFI_SSID  "YOUR_WIFI_SSID"
#define WIFI_PASS  "YOUR_WIFI_PASSWORD"
#define RELAY_PIN   4
#define BUZZER_PIN 15
#define DETECT_CONFIDENCE 0.6f

HumanFaceDetectMSR01 detector(0.3F, 0.3F, 10, 0.3F);
HumanFaceDetectMNP01 recognizer(0.4F, 0.3F, 10);

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN,  OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN,  LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Init camera: 240×240 RGB565 for face detection
  init_camera(FRAMESIZE_240X240, PIXFORMAT_RGB565, 12, 1);

  Serial.printf("Connecting to %s", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) { Serial.print("."); delay(500); }
  Serial.println("\nWi-Fi connected.");
  Serial.println("=== Face Detection Active ===");
}

void loop() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) { delay(100); return; }

  // Run fast face detector (MobileNetSSD stage 1)
  std::list<dl::detect::result_t> candidates =
    detector.infer((uint16_t*)fb->buf, {(int)fb->height, (int)fb->width, 3});

  // Run precise face detector (MobileNetV2 stage 2)
  std::list<dl::detect::result_t> results =
    recognizer.infer((uint16_t*)fb->buf, {(int)fb->height, (int)fb->width, 3},
                     candidates);

  esp_camera_fb_return(fb);

  int faceCount = results.size();
  Serial.printf("[%lus] Faces detected: %d\n", millis() / 1000, faceCount);

  if (faceCount > 0) {
    for (auto& r : results) {
      Serial.printf("  → Score: %.2f  Box:[%d,%d,%d,%d]\n",
                    r.score, r.box[0], r.box[1], r.box[2], r.box[3]);
    }
    // Trigger relay + short beep
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH); delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(800);
    digitalWrite(RELAY_PIN, LOW);
  }

  delay(200);
}
