# AI4 — Edge Impulse Setup & Data Collection

This project does NOT use a custom `.ino` file.
Instead, you flash the official Edge Impulse firmware onto the ESP32-CAM.

## Steps

### 1. Create a Project
- Go to [edgeimpulse.com](https://edgeimpulse.com) → sign up free → **Create new project**
- Select **Image classification** as project type

### 2. Install the CLI
```bash
npm install -g edge-impulse-cli   # requires Node.js 14+
```

### 3. Download ESP32-CAM Firmware
- In your project → **Devices** tab → **Connect a new device**
- Select **ESP32** → download `esp32-cam-flash.zip`
- Extract and run `flash_windows.bat` (or `flash_mac.command` / `flash_linux.sh`)
- Connect your programming cable with **IO0 bridged to GND** before flashing

### 4. Connect to Edge Impulse
```bash
edge-impulse-daemon
```
- Log in with your Edge Impulse credentials
- Your ESP32-CAM will appear as a connected device

### 5. Collect Images
- Go to **Data Acquisition** in the Edge Impulse dashboard
- Live camera feed appears — set label → click **Start sampling**
- Aim for **50–200 images per class**
- Recommended: 80% training / 20% testing split

### 6. Create Impulse
- **Create Impulse** → Add **Image** input block (96×96 px, Grayscale)
- Add **Image** processing block → Add **Transfer Learning** learning block
- Click **Save Impulse**

### 7. Train Model
- **Image** tab → Generate features
- **Transfer Learning** tab → Train (MobileNetV2 0.35, 20–50 epochs)
- Check validation accuracy — target >85%

### 8. Deploy to Arduino
- **Deployment** tab → **Arduino Library** → **Build**
- Download the `.zip` file
- In Arduino IDE: **Sketch → Include Library → Add .ZIP Library**
- The library name matches your project name

## Tips
- Collect images in the **same lighting** as deployment environment
- Use **Data augmentation** (flip, brightness) if you have < 100 images/class
- Run **Model Testing** before deployment to check real-world accuracy
