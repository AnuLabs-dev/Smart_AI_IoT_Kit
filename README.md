# 🤖 Smart IoT & AI Kit — Complete Arduino Code Repository

> **ESP32-CAM based IoT & AI Vision projects** — sensors, actuators, OLED, RFID, IoT cloud, Edge Impulse machine learning, and on-device AI.

---

## 📦 What's in this Repository

| Folder | Contents |
|--------|----------|
| `01_sensors/` | DHT11, BMP280, Ultrasonic, PIR, Soil, Rain, Gas, Flame, NTC, Voltage |
| `02_actuators/` | Servo, Stepper, Relay, RGB LED, Buzzer, Potentiometer |
| `03_display_input/` | 128×64 OLED, RFID RC522, Push Button |
| `04_iot_projects/` | 5 complete IoT builds with Wi-Fi & cloud |
| `05_ai_vision/` | 8 AI/ML vision projects including Edge Impulse |
| `docs/` | Wiring diagrams reference |

---

## 🔧 Hardware Required

- **ESP32-CAM** (AI Thinker module)
- **USB-to-Serial programming cable** (included in kit)
- All sensors and modules listed in the kit

---

## 💻 Software Setup

### 1. Install Arduino IDE 2.x
Download from [arduino.cc/en/software](https://www.arduino.cc/en/software)

### 2. Add ESP32 Board Support
In Arduino IDE → **File → Preferences**, add to *Additional boards manager URLs*:
```
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
```
Then: **Tools → Board → Boards Manager** → search `esp32` → install **ESP32 by Espressif Systems**

### 3. Select Board
**Tools → Board → ESP32 Arduino → AI Thinker ESP32-CAM**

### 4. Install Required Libraries
Go to **Sketch → Include Library → Manage Libraries** and install:

| Library | Used In |
|---------|---------|
| `DHT sensor library` by Adafruit | 01_sensors/DHT11 |
| `Adafruit BMP280 Library` | 01_sensors/BMP280 |
| `Adafruit SSD1306` | 03_display_input/OLED |
| `Adafruit GFX Library` | 03_display_input/OLED |
| `MFRC522` by GithubCommunity | 03_display_input/RFID |
| `ESP32Servo` | 02_actuators/Servo |
| `Stepper` (built-in) | 02_actuators/Stepper |

---

## ⚡ Uploading to ESP32-CAM

The ESP32-CAM **does not have a built-in USB port**. Use the included programming cable:

```
Programming Cable TX  →  ESP32-CAM RX0 (GPIO 3)
Programming Cable RX  →  ESP32-CAM TX0 (GPIO 1)
Programming Cable GND →  ESP32-CAM GND
Programming Cable 5V  →  ESP32-CAM 5V
IO0 (GPIO 0)          →  GND   ← REQUIRED for upload mode
```

**Upload steps:**
1. Connect IO0 to GND
2. Press RESET button on ESP32-CAM
3. Click Upload in Arduino IDE
4. After "Done uploading" — remove IO0-GND bridge
5. Press RESET again to run the sketch

---

## 🗂️ Project Index

### 🌡️ Sensors
| # | Project | Folder |
|---|---------|--------|
| S1 | DHT11 Temperature & Humidity | `01_sensors/DHT11/` |
| S2 | BMP280 Pressure & Altitude | `01_sensors/BMP280/` |
| S3 | HC-SR04 Ultrasonic Distance | `01_sensors/Ultrasonic/` |
| S4 | PIR Motion Detection | `01_sensors/PIR_Motion/` |
| S5 | Soil Moisture Monitor | `01_sensors/Soil_Moisture/` |
| S6 | Rain Sensor | `01_sensors/Rain_Sensor/` |
| S7 | Gas Sensor (MQ-2/MQ-135) | `01_sensors/Gas_Sensor/` |
| S8 | Flame Sensor | `01_sensors/Flame_Sensor/` |
| S9 | NTC Thermistor | `01_sensors/NTC_Sensor/` |
| S10 | Voltage Sensor | `01_sensors/Voltage_Sensor/` |

### ⚙️ Actuators
| # | Project | Folder |
|---|---------|--------|
| A1 | Servo Motor Sweep | `02_actuators/Servo_Motor/` |
| A2 | Stepper Motor Control | `02_actuators/Stepper_Motor/` |
| A3 | Relay Module | `02_actuators/Relay_Module/` |
| A4 | RGB LED Color Mixing | `02_actuators/RGB_LED/` |
| A5 | Active Buzzer Tones | `02_actuators/Buzzer/` |
| A6 | Potentiometer Read | `02_actuators/Potentiometer/` |

### 🖥️ Display & Input
| # | Project | Folder |
|---|---------|--------|
| D1 | 128×64 OLED Display | `03_display_input/OLED_Display/` |
| D2 | RFID RC522 Scanner | `03_display_input/RFID_Access/` |
| D3 | Push Button Debounce | `03_display_input/Push_Button/` |

### 📡 IoT Projects
| # | Project | Folder |
|---|---------|--------|
| P1 | Smart Doorbell + Video Stream | `04_iot_projects/01_Smart_Doorbell/` |
| P2 | Weather Station → ThingSpeak | `04_iot_projects/02_Weather_Station_ThingSpeak/` |
| P3 | RFID Access Control System | `04_iot_projects/03_RFID_Access_Control/` |
| P4 | Smart Plant Monitor + Auto Watering | `04_iot_projects/04_Plant_Auto_Watering/` |
| P5 | Gas Leak & Fire Alarm | `04_iot_projects/05_Gas_Fire_Alarm/` |

### 🧠 AI Vision Projects
| # | Project | Folder |
|---|---------|--------|
| AI1 | Face Detection (ESP-WHO) | `05_ai_vision/01_Face_Detection/` |
| AI2 | TFLite Person Detection | `05_ai_vision/02_TFLite_Person_Detection/` |
| AI3 | Live AI Dashboard | `05_ai_vision/03_AI_Dashboard/` |
| AI4 | Edge Impulse Setup & Data Collection | `05_ai_vision/04_EdgeImpulse_Setup/` |
| AI5 | Image Classifier (Edge Impulse) | `05_ai_vision/05_Image_Classifier/` |
| AI6 | FOMO Object Detection (Edge Impulse) | `05_ai_vision/06_FOMO_Object_Detection/` |
| AI7 | Visual Anomaly Detection (Edge Impulse) | `05_ai_vision/07_Anomaly_Detection/` |
| AI8 | Smart Intruder Alert (Full Build) | `05_ai_vision/08_Smart_Intruder_Alert/` |

---

## 📌 GPIO Quick Reference (ESP32-CAM)

| GPIO | Common Use in This Kit |
|------|----------------------|
| GPIO 0 | Boot/Flash mode (pull LOW to upload) |
| GPIO 1 | UART TX (programming) |
| GPIO 3 | UART RX (programming) |
| GPIO 4 | Onboard Flash LED |
| GPIO 12 | Servo / Relay / Stepper IN1 |
| GPIO 13 | PIR / DHT11 / Stepper IN2 / RFID SS |
| GPIO 14 | Ultrasonic TRIG / Stepper IN3 / I2C SCL |
| GPIO 15 | Buzzer / Stepper IN4 / RFID RST / I2C SDA |
| GPIO 33 | Onboard status LED (active LOW) |
| GPIO 34 | Soil/Gas/Analog sensors (input-only) |

> ⚠️ GPIO 34, 35, 36, 39 are **input-only** — cannot be used as outputs.

---

## 📜 License
MIT License — free to use, modify and share for educational purposes.

## 🙏 Credits
Built for the **Smart IoT & AI Kit** — ESP32-CAM edition.  
Tutorials and manual included with the kit.
