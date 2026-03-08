/**
 * P3 — RFID Access Control System
 * =================================
 * Board : AI Thinker ESP32-CAM
 * Lib   : MFRC522 by GithubCommunity
 *
 * Wiring:
 *   RC522 VCC→3.3V  GND→GND  RST→GPIO15  SDA→GPIO13
 *   RC522 SCK→GPIO14  MOSI→GPIO12  MISO→GPIO2
 *   Relay  IN → GPIO 4 (flash LED pin — acts as relay indicator)
 *   Buzzer S  → GPIO 15 (shared RST — optional, use separate pin if available)
 *
 * How to get card UIDs:
 *   Upload sketch → open Serial Monitor → scan card → copy UID printed.
 *   Paste into AUTHORIZED_UIDS[] below, then re-upload.
 *
 * Access log is printed to Serial with timestamps.
 */
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN     13
#define RST_PIN    15
#define SCK_PIN    14
#define MOSI_PIN   12
#define MISO_PIN    2
#define RELAY_PIN   4   // HIGH = relay ON / LED on

// ── Authorised UIDs (uppercase hex, space-separated) ─────────
const String AUTHORIZED[] = {
  "AB 12 34 CD",    // Master card — replace with your UID
  "EF 56 78 90",    // Secondary card
};
const int N_AUTH = sizeof(AUTHORIZED) / sizeof(AUTHORIZED[0]);

struct LogEntry { String uid; bool granted; uint32_t time; };
LogEntry accessLog[20];
int logIdx = 0;

MFRC522 rfid(SS_PIN, RST_PIN);

String readUID() {
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
  for (int i = 0; i < N_AUTH; i++)
    if (uid == AUTHORIZED[i]) return true;
  return false;
}

void grantAccess() {
  Serial.println(">>> ACCESS GRANTED ✅ — Unlocking for 3 seconds");
  digitalWrite(RELAY_PIN, HIGH);
  delay(3000);
  digitalWrite(RELAY_PIN, LOW);
}

void denyAccess() {
  Serial.println(">>> ACCESS DENIED ❌");
  // Rapid blink to indicate denial
  for (int i = 0; i < 3; i++) {
    digitalWrite(RELAY_PIN, HIGH); delay(100);
    digitalWrite(RELAY_PIN, LOW);  delay(100);
  }
}

void printLog() {
  Serial.println("\n--- Access Log ---");
  for (int i = 0; i < logIdx; i++) {
    Serial.printf("[%us] %s — %s\n",
                  accessLog[i].time / 1000,
                  accessLog[i].uid.c_str(),
                  accessLog[i].granted ? "GRANTED" : "DENIED");
  }
  Serial.println("---");
}

void setup() {
  Serial.begin(115200);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println("=== RFID Access Control System ===");
  Serial.printf("Authorised cards loaded: %d\n", N_AUTH);
  Serial.println("Scan a card...");
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial())  return;

  String uid  = readUID();
  bool   auth = isAuthorized(uid);

  Serial.printf("\n[%us] Card scanned: %s\n", millis() / 1000, uid.c_str());

  if (auth) grantAccess();
  else       denyAccess();

  // Log entry
  if (logIdx < 20) {
    accessLog[logIdx++] = {uid, auth, millis()};
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(1000);
}
