/**
 * D3 — Push Button with Debounce
 * ================================
 * Board : AI Thinker ESP32-CAM
 *
 * Wiring:
 *   Button pin 1 → GPIO 13
 *   Button pin 2 → GND
 *   (Uses internal pull-up — no external resistor needed)
 *
 * Features:
 *   - Software debounce (50 ms)
 *   - Toggle LED on each press
 *   - Single-press, long-press and double-press detection
 */
#define BTN_PIN    13
#define LED_PIN     4   // onboard flash LED

#define DEBOUNCE_MS    50
#define LONG_PRESS_MS 800
#define DOUBLE_GAP_MS 400

bool     ledState       = false;
bool     lastRaw        = HIGH;
bool     stableState    = HIGH;
uint32_t lastDebounce   = 0;
uint32_t pressStart     = 0;
bool     pressing       = false;
uint32_t lastRelease    = 0;
bool     waitDouble     = false;

void handlePress(bool isLong, bool isDouble) {
  if (isDouble) {
    Serial.println("DOUBLE PRESS — both LEDs toggle");
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  } else if (isLong) {
    Serial.println("LONG PRESS — LED ON");
    ledState = true;
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("SHORT PRESS — LED toggle");
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState ? HIGH : LOW);
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("=== Push Button Demo ===");
  Serial.println("Short press = toggle LED");
  Serial.println("Long press  = LED ON");
  Serial.println("Double press = LED OFF");
}

void loop() {
  bool raw = digitalRead(BTN_PIN);

  // Debounce
  if (raw != lastRaw) lastDebounce = millis();
  lastRaw = raw;
  if (millis() - lastDebounce < DEBOUNCE_MS) return;

  // Press detection
  if (stableState == HIGH && raw == LOW) {        // just pressed
    pressing   = true;
    pressStart = millis();
    stableState = raw;
  } else if (stableState == LOW && raw == HIGH) { // just released
    uint32_t held = millis() - pressStart;
    pressing    = false;
    stableState = raw;

    if (held >= LONG_PRESS_MS) {
      handlePress(true, false);
      waitDouble = false;
    } else {
      if (waitDouble && (millis() - lastRelease < DOUBLE_GAP_MS)) {
        handlePress(false, true);
        waitDouble = false;
      } else {
        waitDouble  = true;
        lastRelease = millis();
      }
    }
  }

  // Double-press timeout
  if (waitDouble && (millis() - lastRelease > DOUBLE_GAP_MS)) {
    handlePress(false, false);
    waitDouble = false;
  }
}
