/**
 * AI4 — Edge Impulse Data Collection Helper
 * ===========================================
 * Board : AI Thinker ESP32-CAM
 *
 * This sketch captures JPEG images on button press and
 * prints them as Base64 to Serial — useful for manual
 * data collection when the EI daemon is not available.
 *
 * Use the official EI firmware (see README.md) for the
 * best data collection experience with live labeling.
 *
 * Wiring:
 *   Button → GPIO 13  (triggers capture)
 *   LED    → GPIO 4   (flash during capture)
 */
#include "esp_camera.h"
#include "camera_config.h"
#include "mbedtls/base64.h"

#define BTN_PIN 13
#define LED_PIN  4

int imageCount = 0;

void captureAndPrint() {
  digitalWrite(LED_PIN, HIGH);
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) { Serial.println("[ERROR] Capture failed"); digitalWrite(LED_PIN, LOW); return; }

  // Encode JPEG as Base64 for easy transfer
  size_t base64Len = 0;
  mbedtls_base64_encode(nullptr, 0, &base64Len, fb->buf, fb->len);
  uint8_t* b64buf = (uint8_t*)malloc(base64Len + 1);
  if (b64buf) {
    mbedtls_base64_encode(b64buf, base64Len + 1, &base64Len, fb->buf, fb->len);
    Serial.printf("--- IMAGE %d (%u bytes JPEG, %u bytes b64) ---\n",
                  ++imageCount, fb->len, base64Len);
    Serial.println((char*)b64buf);
    Serial.println("--- END ---");
    free(b64buf);
  }
  esp_camera_fb_return(fb);
  delay(200);
  digitalWrite(LED_PIN, LOW);
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN,  OUTPUT);
  init_camera(FRAMESIZE_96X96, PIXFORMAT_JPEG, 10, 1);
  Serial.println("=== EI Data Collection Helper ===");
  Serial.println("Press button (GPIO 13) to capture image.");
}

void loop() {
  static bool lastBtn = HIGH;
  bool btn = digitalRead(BTN_PIN);
  if (lastBtn == HIGH && btn == LOW) captureAndPrint();
  lastBtn = btn;
  delay(20);
}
