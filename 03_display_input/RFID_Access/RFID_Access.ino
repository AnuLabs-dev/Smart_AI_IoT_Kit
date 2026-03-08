/**
 * D2 — RFID RC522 Scanner & Access Logger
 * =========================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : MFRC522 by GithubCommunity
 *
 * Wiring (SPI):
 *   RC522 VCC  → 3.3V  (NEVER 5V!)
 *   RC522 GND  → GND
 *   RC522 RST  → GPIO 15
 *   RC522 SDA  → GPIO 13  (SS/CS)
 *   RC522 SCK  → GPIO 14
 *   RC522 MOSI → GPIO 12
 *   RC522 MISO → GPIO 2
 *
 * How to find your card UID:
 *   1. Upload this sketch.
 *   2. Open Serial Monitor at 115200.
 *   3. Scan any card — copy the UID printed.
 *   4. Add it to AUTHORIZED_UIDS below.
 */
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN   13
#define RST_PIN  15
#define SCK_PIN  14
#define MOSI_PIN 12
#define MISO_PIN  2
#define RELAY_PIN 4   // Use flash LED GPIO as indicator
#define BUZZER_PIN 15 // Reuse RST if no buzzer; wire separately if available

MFRC522 rfid(SS_PIN, RST_PIN);

// Add your authorised card UIDs here (uppercase, space-separated hex bytes)
const String AUTHORIZED_UIDS[] = {
  "AB 12 34 CD",   // Card 1 — replace with your UID
  "12 AB CD EF",   // Card 2
};
const int NUM_AUTH = sizeof(AUTHORIZED_UIDS) / sizeof(AUTHORIZED_UIDS[0]);

String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

bool isAuthorized(const String& uid) {
  for (int i = 0; i < NUM_AUTH; i++) {
    if (uid == AUTHORIZED_UIDS[i]) return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("=== RFID RC522 Access Control ===");
  Serial.println("Scan a card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial())  return;

  String uid = getUID();
  bool   auth = isAuthorized(uid);

  Serial.printf("[%lus] UID: %s  →  %s\n",
                millis() / 1000, uid.c_str(),
                auth ? "ACCESS GRANTED ✅" : "ACCESS DENIED ❌");

  if (auth) {
    digitalWrite(RELAY_PIN, HIGH);  // activate relay / green LED
    delay(3000);
    digitalWrite(RELAY_PIN, LOW);
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(500);
}
