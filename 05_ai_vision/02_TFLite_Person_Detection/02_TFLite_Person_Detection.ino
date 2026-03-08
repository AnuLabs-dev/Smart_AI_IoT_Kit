/**
 * AI2 — TensorFlow Lite Person Detection (Micro)
 * ================================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : TensorFlowLite_ESP32 (install via Library Manager)
 *         Search "TensorFlow Lite Micro for ESP" by Espressif
 *
 * Uses the pre-trained person detection model (96×96 grayscale input).
 * No training required — detects whether a person is in frame.
 *
 * Wiring:
 *   LED/Relay → GPIO 4   Buzzer → GPIO 15
 *
 * Memory: ~100KB tensor arena — PSRAM required for reliable operation.
 * Speed: ~3–5 FPS at 96×96 grayscale.
 */
#include "esp_camera.h"
#include "camera_config.h"
#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
// Person detection model (included in TFLite_ESP32 library)
#include "person_detect_model_data.h"

#define LED_PIN    4
#define BUZZER_PIN 15

// ── TFLite setup ──────────────────────────────────────────────
const int kTensorArenaSize = 100 * 1024;  // 100 KB
static uint8_t tensor_arena[kTensorArenaSize];

const tflite::Model*          model       = nullptr;
tflite::MicroInterpreter*     interpreter = nullptr;
TfLiteTensor*                 input       = nullptr;
TfLiteTensor*                 output      = nullptr;

// Person index in model output tensor
const int kPersonIndex   = 1;
const int kNoPersonIndex = 0;
const float kThreshold   = 0.6f;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN,    OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN,    LOW);
  digitalWrite(BUZZER_PIN, LOW);

  // Init camera at 96×96 grayscale
  if (init_camera(FRAMESIZE_96X96, PIXFORMAT_GRAYSCALE, 12, 1) != ESP_OK) {
    Serial.println("[ERROR] Camera init failed"); while (1) delay(500);
  }

  // Load model
  model = tflite::GetModel(g_person_detect_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("[ERROR] Model schema mismatch"); while (1) delay(500);
  }

  static tflite::MicroMutableOpResolver<5> resolver;
  resolver.AddAveragePool2D();
  resolver.AddConv2D();
  resolver.AddDepthwiseConv2D();
  resolver.AddReshape();
  resolver.AddSoftmax();

  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize);
  interpreter = &static_interpreter;
  interpreter->AllocateTensors();

  input  = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("=== TFLite Person Detection ===");
  Serial.printf("Input  tensor: [%d,%d,%d]\n",
                input->dims->data[1], input->dims->data[2], input->dims->data[3]);
}

void loop() {
  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) { delay(100); return; }

  // Copy grayscale frame into TFLite input tensor
  // Model expects INT8 (quantized) in range [-128, 127]
  if (fb->len == (size_t)(input->dims->data[1] * input->dims->data[2])) {
    for (size_t i = 0; i < fb->len; i++) {
      input->data.int8[i] = (int8_t)fb->buf[i] - 128;
    }
  }
  esp_camera_fb_return(fb);

  // Run inference
  TfLiteStatus status = interpreter->Invoke();
  if (status != kTfLiteOk) { Serial.println("[ERROR] Inference failed"); return; }

  // Read scores (INT8 quantized output)
  int8_t person_score   = output->data.int8[kPersonIndex];
  int8_t noperson_score = output->data.int8[kNoPersonIndex];

  // Dequantize to 0–1
  float p_score = (person_score   - output->params.zero_point) * output->params.scale;
  float n_score = (noperson_score - output->params.zero_point) * output->params.scale;

  bool personDetected = (p_score > kThreshold);

  Serial.printf("[%lus] Person: %.2f  No-person: %.2f  → %s\n",
                millis() / 1000, p_score, n_score,
                personDetected ? "PERSON DETECTED 👤" : "clear");

  if (personDetected) {
    digitalWrite(LED_PIN,    HIGH);
    digitalWrite(BUZZER_PIN, HIGH); delay(300);
    digitalWrite(BUZZER_PIN, LOW);
    delay(700);
    digitalWrite(LED_PIN, LOW);
  }

  delay(200);
}
