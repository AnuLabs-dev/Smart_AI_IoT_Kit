/**
 * camera_config.h
 * ---------------
 * Shared AI Thinker ESP32-CAM pin configuration.
 * Include this header in any sketch that uses the camera.
 *
 * Usage:
 *   #include "camera_config.h"
 *   esp_err_t err = init_camera(FRAMESIZE_QVGA, PIXFORMAT_JPEG);
 */

#pragma once
#include "esp_camera.h"

// ── AI Thinker ESP32-CAM pin map ──────────────────────────────
#define CAM_PIN_PWDN    32
#define CAM_PIN_RESET   -1
#define CAM_PIN_XCLK     0
#define CAM_PIN_SIOD    26
#define CAM_PIN_SIOC    27
#define CAM_PIN_D7      35
#define CAM_PIN_D6      34
#define CAM_PIN_D5      39
#define CAM_PIN_D4      36
#define CAM_PIN_D3      21
#define CAM_PIN_D2      19
#define CAM_PIN_D1      18
#define CAM_PIN_D0       5
#define CAM_PIN_VSYNC   25
#define CAM_PIN_HREF    23
#define CAM_PIN_PCLK    22
#define CAM_PIN_FLASH    4   // Onboard white flash LED

// ── Initialise camera ─────────────────────────────────────────
// framesize : FRAMESIZE_96X96, FRAMESIZE_QQVGA, FRAMESIZE_QVGA,
//             FRAMESIZE_VGA, FRAMESIZE_SVGA, FRAMESIZE_UXGA …
// pixformat : PIXFORMAT_JPEG, PIXFORMAT_RGB565, PIXFORMAT_GRAYSCALE
esp_err_t init_camera(framesize_t framesize  = FRAMESIZE_QVGA,
                      pixformat_t pixformat  = PIXFORMAT_JPEG,
                      int         jpeg_qual  = 12,
                      int         fb_count   = 1) {

  camera_config_t config;
  config.ledc_channel  = LEDC_CHANNEL_0;
  config.ledc_timer    = LEDC_TIMER_0;
  config.pin_d0        = CAM_PIN_D0;
  config.pin_d1        = CAM_PIN_D1;
  config.pin_d2        = CAM_PIN_D2;
  config.pin_d3        = CAM_PIN_D3;
  config.pin_d4        = CAM_PIN_D4;
  config.pin_d5        = CAM_PIN_D5;
  config.pin_d6        = CAM_PIN_D6;
  config.pin_d7        = CAM_PIN_D7;
  config.pin_xclk      = CAM_PIN_XCLK;
  config.pin_pclk      = CAM_PIN_PCLK;
  config.pin_vsync     = CAM_PIN_VSYNC;
  config.pin_href      = CAM_PIN_HREF;
  config.pin_sscb_sda  = CAM_PIN_SIOD;
  config.pin_sscb_scl  = CAM_PIN_SIOC;
  config.pin_pwdn      = CAM_PIN_PWDN;
  config.pin_reset     = CAM_PIN_RESET;
  config.xclk_freq_hz  = 20000000;
  config.pixel_format  = pixformat;
  config.frame_size    = framesize;
  config.jpeg_quality  = jpeg_qual;  // 0–63, lower = higher quality
  config.fb_count      = fb_count;
  config.grab_mode     = CAMERA_GRAB_WHEN_EMPTY;

  // PSRAM detected → allow larger buffers
  if (psramFound()) {
    config.fb_location = CAMERA_FB_IN_PSRAM;
  } else {
    config.fb_location = CAMERA_FB_IN_DRAM;
    if (framesize > FRAMESIZE_QVGA) {
      Serial.println("[Camera] WARN: No PSRAM — forcing QVGA");
      config.frame_size = FRAMESIZE_QVGA;
    }
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[Camera] Init failed: 0x%x\n", err);
  } else {
    Serial.println("[Camera] Initialized OK");
  }
  return err;
}
