/**
 * AI6 — FOMO Object Detection (Edge Impulse)
 * ============================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : Your Edge Impulse Arduino library with FOMO model
 *
 * FOMO (Faster Objects More Objects):
 *   - Ultra-lightweight object detection for microcontrollers
 *   - ~30 FPS at 96×96 on ESP32-CAM
 *   - Outputs object centroids (not full bounding boxes)
 *   - Train with bounding-box labels on edgeimpulse.com
 *
 * BEFORE UPLOADING:
 *   1. Train FOMO model on Edge Impulse with bounding box labels.
 *      Learning block: Object Detection (Images) → FOMO MobileNetV2 0.35
 *   2. Deploy as Arduino Library → install in Arduino IDE.
 *   3. Replace header below with your project name.
 *
 * Wiring:
 *   Relay → GPIO 12   Buzzer → GPIO 15
 *   (Triggers when detected object count > MAX_ALLOWED)
 */

// ── REPLACE with your Edge Impulse project library header ────
#include <YOUR_PROJECT_NAME_inferencing.h>
// ─────────────────────────────────────────────────────────────

#include "esp_camera.h"
#include "camera_config.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"

#define RELAY_PIN    12
#define BUZZER_PIN   15
#define BB_THRESHOLD 0.5f   // bounding box confidence threshold
#define MAX_ALLOWED  3      // trigger alert if more than this many objects detected

// ── Feature buffer ────────────────────────────────────────────
static uint8_t* rgb_buf = nullptr;

// ── Feature extraction callback ───────────────────────────────
int ei_camera_get_data(size_t offset, size_t length, float* out_ptr) {
  // FOMO expects RGB888 normalised to 0–255 packed as float
  size_t pixel_ix = offset * 3;
  for (size_t i = 0; i < length; i++) {
    out_ptr[i] = (rgb_buf[pixel_ix]     << 16) |  // R
                 (rgb_buf[pixel_ix + 1] <<  8) |  // G
                 (rgb_buf[pixel_ix + 2]);          // B
    pixel_ix += 3;
  }
  return 0;
}

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN,  OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN,  HIGH); // active LOW relay off
  digitalWrite(BUZZER_PIN, LOW);

  // Allocate RGB buffer (96×96×3)
  size_t buf_size = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT * 3;
  rgb_buf = (uint8_t*)heap_caps_malloc(buf_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
  if (!rgb_buf) rgb_buf = (uint8_t*)malloc(buf_size);
  if (!rgb_buf) { Serial.println("[ERROR] RGB buffer alloc failed"); while (1) delay(500); }

  // Camera in JPEG mode — we'll decode each frame
  if (init_camera(FRAMESIZE_96X96, PIXFORMAT_JPEG, 12, 1) != ESP_OK) {
    Serial.println("[ERROR] Camera init failed"); while (1) delay(500);
  }

  Serial.println("=== FOMO Object Detection ===");
  Serial.printf("Model input: %dx%d  Classes: %d\n",
                EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT,
                EI_CLASSIFIER_LABEL_COUNT);
}

void loop() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) { delay(100); return; }

  // Convert JPEG → RGB888 → resize to 96×96
  bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, rgb_buf);
  esp_camera_fb_return(fb);

  if (!converted) { Serial.println("[WARN] JPEG decode failed"); return; }

  // Resize if frame is not already 96×96
  // (camera set to 96×96 so this should be a no-op in most cases)

  // Build EI signal
  signal_t signal;
  signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  signal.get_data     = &ei_camera_get_data;

  // Run FOMO inference
  ei_impulse_result_t result;
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
  if (err != EI_IMPULSE_OK) { Serial.printf("[ERROR] Classifier: %d\n", err); return; }

  // Count and print detections
  uint32_t detected = 0;
  Serial.printf("[%lus] Detections (%.1f ms):\n",
                millis() / 1000, result.timing.classification);

  for (auto& bb : result.bounding_boxes) {
    if (bb.value < BB_THRESHOLD) continue;
    detected++;
    Serial.printf("  [%u] %-15s conf:%.2f  x:%3d y:%3d w:%3d h:%3d\n",
                  detected, bb.label, bb.value,
                  bb.x, bb.y, bb.width, bb.height);
  }
  Serial.printf("  Total detected: %u\n\n", detected);

  // Trigger relay if count exceeds limit
  if (detected > MAX_ALLOWED) {
    Serial.printf("⚠️  ALERT: %u objects detected (max: %d)\n", detected, MAX_ALLOWED);
    digitalWrite(RELAY_PIN, LOW);   // relay ON
    for (int i = 0; i < 3; i++) {
      digitalWrite(BUZZER_PIN, HIGH); delay(150);
      digitalWrite(BUZZER_PIN, LOW);  delay(150);
    }
    delay(2000);
    digitalWrite(RELAY_PIN, HIGH);  // relay OFF
  }

  delay(100);
}
