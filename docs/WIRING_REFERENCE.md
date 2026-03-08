# Wiring Reference — Smart IoT & AI Kit

## ESP32-CAM GPIO Summary

| GPIO | Direction | Used In |
|------|-----------|---------|
| 0    | I/O (Boot) | Programming — pull LOW to flash |
| 1    | TX | Serial / Programming |
| 3    | RX | Serial / Programming |
| 2    | MISO (SPI) | RFID RC522 |
| 4    | OUTPUT | Flash LED (onboard), also used as relay/relay indicator |
| 5    | — | Camera internal |
| 12   | OUTPUT | Servo / Relay / Stepper IN1 / RFID MOSI |
| 13   | I/O | PIR, DHT11, Stepper IN2, RFID SS, Button, Soil D0, etc. |
| 14   | OUTPUT | Ultrasonic TRIG, Stepper IN3, I2C SCL, RFID SCK |
| 15   | I/O | Buzzer, Stepper IN4, RFID RST, I2C SDA |
| 18–22| — | Camera internal |
| 25–27| — | Camera internal |
| 32   | OUTPUT | Camera PWDN (internal) |
| 33   | OUTPUT | Onboard status LED (active LOW) |
| 34   | INPUT only | Soil A0, Gas A0, Flame A0, Rain A0, Voltage, NTC |
| 35–39| INPUT only | Camera data pins |

> ⚠️ GPIOs 34, 35, 36, 39 are **input-only** — they cannot drive outputs.
> ⚠️ GPIO 6–11 are connected to SPI flash — **never use these**.

---

## Sensor Wiring Quick Reference

### DHT11 (GPIO 13)
```
DHT11 VCC  →  3.3V
DHT11 DATA →  GPIO 13  [+ 10kΩ pull-up to 3.3V]
DHT11 GND  →  GND
```

### BMP280 I2C (GPIO 14/15)
```
BMP280 VCC  →  3.3V      BMP280 GND  →  GND
BMP280 SCL  →  GPIO 14   BMP280 SDA  →  GPIO 15
I2C address: 0x76 (SDO to GND) or 0x77 (SDO to VCC)
```

### HC-SR04 Ultrasonic (GPIO 12/13)
```
HC-SR04 VCC   →  5V
HC-SR04 TRIG  →  GPIO 12
HC-SR04 ECHO  →  1kΩ → GPIO 13 → 2kΩ → GND  (voltage divider!)
HC-SR04 GND   →  GND
```

### PIR Motion HC-SR501 (GPIO 13)
```
PIR VCC  →  5V
PIR OUT  →  GPIO 13
PIR GND  →  GND
```

### Soil Moisture (GPIO 34 + 13)
```
Soil VCC  →  3.3V    Soil GND  →  GND
Soil A0   →  GPIO 34 (analog)
Soil D0   →  GPIO 13 (digital)
```

### Gas Sensor MQ-2/135 (GPIO 34 + 13)
```
Gas VCC  →  5V      Gas GND  →  GND
Gas A0   →  GPIO 34 Gas D0   →  GPIO 13
```

### Flame Sensor (GPIO 34 + 12)
```
Flame VCC  →  3.3V    Flame GND  →  GND
Flame A0   →  GPIO 34 Flame D0   →  GPIO 12
```

### NTC Thermistor (GPIO 34)
```
3.3V → [10kΩ] → GPIO 34 → [NTC 10k] → GND
```

### Voltage Sensor (GPIO 34)
```
Sensor +  →  Input positive (0–25V)
Sensor -  →  Input GND (shared with ESP32-CAM GND)
Sensor S  →  GPIO 34
```

---

## Actuator Wiring Quick Reference

### SG90 Servo (GPIO 12)
```
Servo Red (VCC)    →  5V
Servo Brown (GND)  →  GND
Servo Orange (SIG) →  GPIO 12
```

### 28BYJ-48 Stepper + ULN2003 (GPIO 12–15)
```
ULN2003 IN1  →  GPIO 12   ULN2003 IN2  →  GPIO 13
ULN2003 IN3  →  GPIO 14   ULN2003 IN4  →  GPIO 15
ULN2003 VCC  →  5V        ULN2003 GND  →  GND
```

### Relay Module (GPIO 12)
```
Relay VCC  →  5V
Relay GND  →  GND
Relay IN   →  GPIO 12  (active LOW: LOW=ON, HIGH=OFF)
Relay COM  →  Load common
Relay NO   →  Load (normally open — default OFF)
```

### RGB LED (GPIO 12/13/14)
```
RGB R   →  GPIO 12 (via 220Ω)
RGB G   →  GPIO 13 (via 220Ω)
RGB B   →  GPIO 14 (via 220Ω)
RGB GND →  GND
```

### Buzzer (GPIO 15)
```
Buzzer VCC  →  3.3V
Buzzer GND  →  GND
Buzzer S    →  GPIO 15
```

---

## Display / Input Wiring

### 128×64 OLED (I2C, GPIO 14/15)
```
OLED VCC  →  3.3V      OLED GND  →  GND
OLED SCL  →  GPIO 14   OLED SDA  →  GPIO 15
I2C address: 0x3C
```

### RC522 RFID (SPI, GPIO 2/12/13/14/15)
```
RC522 VCC   →  3.3V  (NEVER 5V!)
RC522 GND   →  GND
RC522 RST   →  GPIO 15
RC522 SDA   →  GPIO 13 (SS)
RC522 SCK   →  GPIO 14
RC522 MOSI  →  GPIO 12
RC522 MISO  →  GPIO 2
```

### Push Button (GPIO 13)
```
Button pin 1  →  GPIO 13
Button pin 2  →  GND
(Uses INPUT_PULLUP — no external resistor needed)
```

---

## Programming Cable Wiring

```
Cable TX  →  ESP32-CAM RX0 (GPIO 3)
Cable RX  →  ESP32-CAM TX0 (GPIO 1)
Cable GND →  ESP32-CAM GND
Cable 5V  →  ESP32-CAM 5V
IO0       →  GND  ← Bridge for upload, remove after
```

**Upload procedure:**
1. Bridge IO0 to GND → connect USB
2. Open Serial Monitor → click Upload
3. When "Connecting..." appears → press RESET briefly
4. After "Done uploading" → remove IO0-GND bridge → press RESET
