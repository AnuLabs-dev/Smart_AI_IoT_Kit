/**
 * AI5 — Custom Image Classifier (Edge Impulse)
 * =============================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : Your Edge Impulse Arduino library (exported from ei.com)
 *
 * BEFORE UPLOADING:
 *   1. Train your model on Edge Impulse (see AI4 README).
 *   2. Deployment → Arduino Library → Build → download .zip
 *   3. Arduino IDE → Sketch → Include Library → Add .ZIP Library
 *   4. Replace "YOUR_PROJECT_NAME_inferencing.h" with your actual header name.
 *      It matches your EI project name (lowercase, underscores).
 *
 * Model settings used (must match what you trained):
 *   Input:  96×96 px, Grayscale
 *   Output: Softmax classification (N classes)
 *
 * Wiring:
 *   LED/Relay → GPIO 4   Buzzer → GPIO 15
 *   (Output triggers when top class confidence > THRESHOLD)
 *
 * Example use cases:
 *   - Mask / No-Mask detector
 *   - Open / Closed door
 *   - Waste sorting (plastic / paper / metal)
 *   - Gesture recognition (thumbs up / thumbs down)
 */

// ── REPLACE with your Edge Impulse project library header ────
#include <YOUR_PROJECT_NAME_inferencing.h>
// ─────────────────────────────────────────────────────────────

#include "esp_camera.h"
#include "camera_config.h"

#define LED_PIN       4
#define BUZZER_PIN   15
#define THRESHOLD    0.75f    // minimum confidence to act on prediction
#define TRIGGER_LABEL "mask"  // change to whichever class should trigger relay

// ── Feature extraction callback ───────────────────────────────
static camera_fb_t* current_fb = nullptr;

int ei_camera_get_data(size_t offset, size_t length, float* out_ptr) {
  // Grayscale: single channel, normalise to 0.0–1.0
  size_t pixel_ix = offset;
  for (size_t i = 0; i < length; i++) {
    out_ptr[i] = current_fb->buf[pixel_ix++] / 255.0f;
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN,    LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Must match Edge Impulse input block settings
  if (init_camera(FRAMESIZE_96X96, PIXFORMAT_GRAYSCALE, 12, 1) != ESP_OK) {
    Serial.println("[ERROR] Camera init failed"); while (1) delay(500);
  }

  Serial.println("=== Edge Impulse Image Classifier ===");
  Serial.printf("Model: %d classes, input %dx%d\n",
                EI_CLASSIFIER_LABEL_COUNT,
                EI_CLASSIFIER_INPUT_WIDTH,
                EI_CLASSIFIER_INPUT_HEIGHT);
}

void loop() {
  current_fb = esp_camera_fb_get();
  if (!current_fb) { delay(100); return; }

  // Validate frame size matches model expectations
  size_t expected = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  if (current_fb->len != expected) {
    Serial.printf("[WARN] Frame size mismatch: got %u, need %u\n",
                  current_fb->len, expected);
    esp_camera_fb_return(current_fb);
    return;
  }

  // Build signal for Edge Impulse
  signal_t signal;
  signal.total_length = expected;
  signal.get_data     = &ei_camera_get_data;

  // Run classifier
  ei_impulse_result_t result;
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
  esp_camera_fb_return(current_fb);
  current_fb = nullptr;

  if (err != EI_IMPULSE_OK) {
    Serial.printf("[ERROR] run_classifier failed: %d\n", err);
    return;
  }

  // Print all class scores
  Serial.printf("[%lus] Inference (%.1f ms):\n",
                millis() / 1000, result.timing.classification);

  float topConf  = 0;
  String topLabel = "";
  for (int i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
    float conf = result.classification[i].value;
    const char* lbl = result.classification[i].label;
    Serial.printf("  %-20s %.2f%%\n", lbl, conf * 100);
    if (conf > topConf) { topConf = conf; topLabel = lbl; }
  }
  Serial.printf("  → TOP: %s (%.0f%%)\n\n", topLabel.c_str(), topConf * 100);

  // Trigger output if top prediction matches target class and exceeds threshold
  if (topLabel == TRIGGER_LABEL && topConf >= THRESHOLD) {
    Serial.println("  ✅ TRIGGER LABEL DETECTED — activating output");
    digitalWrite(LED_PIN,    HIGH);
    digitalWrite(BUZZER_PIN, HIGH); delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    delay(700);
    digitalWrite(LED_PIN, LOW);
  }

  delay(500); // ~2 FPS
}
