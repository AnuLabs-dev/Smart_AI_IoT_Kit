/**
 * AI7 — Visual Anomaly Detection (Edge Impulse GMM)
 * ===================================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : Your Edge Impulse Arduino library with anomaly model
 *
 * Train only on NORMAL/GOOD images — no defect examples needed.
 * The model learns what "normal" looks like and flags deviations.
 *
 * BEFORE UPLOADING:
 *   1. Collect 40+ images of the NORMAL state only (on Edge Impulse).
 *   2. Create Impulse: Image (96×96) → Image processing → Anomaly Detection (GMM)
 *   3. Train → test → Deploy as Arduino Library → install.
 *   4. Replace header with your project name below.
 *   5. Set ANOMALY_THRESHOLD after testing with known-defect images.
 *
 * Use Cases:
 *   - PCB defect detection (missing components, solder bridges)
 *   - Product quality inspection on a production line
 *   - Tamper detection (is an object in its normal position?)
 *   - Spill / stain detection
 *
 * Wiring:
 *   Buzzer → GPIO 15   Flash LED → GPIO 4  Relay → GPIO 12
 */

// ── REPLACE with your Edge Impulse project library header ────
#include <YOUR_PROJECT_NAME_inferencing.h>
// ─────────────────────────────────────────────────────────────

#include "esp_camera.h"
#include "camera_config.h"

#define LED_PIN          4
#define BUZZER_PIN      15
#define RELAY_PIN       12
#define ANOMALY_THRESHOLD 0.30f   // Tune this! Higher = less sensitive

// History buffer for running average (helps filter one-off noise)
#define HISTORY_SIZE 3
float scoreHistory[HISTORY_SIZE] = {0};
int   historyIdx = 0;

static camera_fb_t* current_fb = nullptr;

int ei_camera_get_data(size_t offset, size_t length, float* out_ptr) {
  for (size_t i = 0; i < length; i++) {
    out_ptr[i] = current_fb->buf[offset + i] / 255.0f;
  }
  return 0;
}

float averageScore() {
  float sum = 0;
  for (int i = 0; i < HISTORY_SIZE; i++) sum += scoreHistory[i];
  return sum / HISTORY_SIZE;
}

void triggerAlert(float score) {
  Serial.printf("🚨 ANOMALY! Score: %.4f (threshold: %.2f)\n",
                score, ANOMALY_THRESHOLD);
  for (int i = 0; i < 4; i++) {
    digitalWrite(LED_PIN,    HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(150);
    digitalWrite(LED_PIN,    LOW);
    digitalWrite(BUZZER_PIN, LOW);
    delay(150);
  }
  // Optionally activate relay (e.g., stop conveyor belt)
  digitalWrite(RELAY_PIN, LOW);
  delay(3000);
  digitalWrite(RELAY_PIN, HIGH);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_PIN,  OUTPUT);
  digitalWrite(LED_PIN,    LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(RELAY_PIN,  HIGH); // relay off (active LOW)

  if (init_camera(FRAMESIZE_96X96, PIXFORMAT_GRAYSCALE, 12, 1) != ESP_OK) {
    Serial.println("[ERROR] Camera init failed"); while (1) delay(500);
  }

  Serial.println("=== Visual Anomaly Detection ===");
  Serial.printf("Threshold: %.2f  History: %d frames\n",
                ANOMALY_THRESHOLD, HISTORY_SIZE);
  Serial.println("Position camera over object to monitor. Starting in 3s...");
  delay(3000);
}

void loop() {
  current_fb = esp_camera_fb_get();
  if (!current_fb) { delay(100); return; }

  size_t expected = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
  if (current_fb->len != expected) {
    esp_camera_fb_return(current_fb);
    return;
  }

  signal_t signal;
  signal.total_length = expected;
  signal.get_data     = &ei_camera_get_data;

  ei_impulse_result_t result;
  EI_IMPULSE_ERROR err = run_classifier(&signal, &result, false);
  esp_camera_fb_return(current_fb);
  current_fb = nullptr;

  if (err != EI_IMPULSE_OK) { Serial.printf("[ERROR] Classifier: %d\n", err); return; }

  float score = result.anomaly;
  scoreHistory[historyIdx++ % HISTORY_SIZE] = score;
  float avgScore = averageScore();

  Serial.printf("[%lus] Raw anomaly: %.4f  Avg: %.4f  → %s\n",
                millis() / 1000, score, avgScore,
                avgScore > ANOMALY_THRESHOLD ? "ANOMALY ⚠️" : "NORMAL ✅");

  if (avgScore > ANOMALY_THRESHOLD) {
    triggerAlert(avgScore);
    // Reset history after alert
    for (int i = 0; i < HISTORY_SIZE; i++) scoreHistory[i] = 0;
    historyIdx = 0;
  }

  delay(500);
}
