/**
 * A4 — RGB LED Module (Common Cathode)
 * ======================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   RGB R   → GPIO 12 (via 220Ω)
 *   RGB G   → GPIO 13 (via 220Ω)
 *   RGB B   → GPIO 14 (via 220Ω)
 *   RGB GND → GND
 *
 * ESP32-CAM uses LEDC hardware PWM (analogWrite not supported natively).
 */
#define R_PIN 12
#define G_PIN 13
#define B_PIN 14

// LEDC channels — one per PWM pin
#define CH_R 0
#define CH_G 1
#define CH_B 2

void setupPWM() {
  ledcSetup(CH_R, 5000, 8); ledcAttachPin(R_PIN, CH_R);
  ledcSetup(CH_G, 5000, 8); ledcAttachPin(G_PIN, CH_G);
  ledcSetup(CH_B, 5000, 8); ledcAttachPin(B_PIN, CH_B);
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  ledcWrite(CH_R, r);
  ledcWrite(CH_G, g);
  ledcWrite(CH_B, b);
}

struct Color { uint8_t r, g, b; const char* name; };
Color colors[] = {
  {255,   0,   0, "Red"},
  {  0, 255,   0, "Green"},
  {  0,   0, 255, "Blue"},
  {255, 255,   0, "Yellow"},
  {  0, 255, 255, "Cyan"},
  {255,   0, 255, "Magenta"},
  {255, 165,   0, "Orange"},
  {255, 255, 255, "White"},
  {  0,   0,   0, "Off"},
};

void setup() {
  Serial.begin(115200);
  setupPWM();
  Serial.println("=== RGB LED Color Cycle ===");
}

void loop() {
  for (auto& c : colors) {
    Serial.printf("Color: %s\n", c.name);
    setColor(c.r, c.g, c.b);
    delay(1000);
  }
}
