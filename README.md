# Elyssa Dev Board

ESP32-S3FN8 based development board by Moovma.

## Features

- ESP32-S3FN8
- 8 MB Flash
- USB Programming
- RGB LED (red, green, blue channels)
- 6-axis motion sensor (IMU: accelerometer + gyroscope) on a dedicated I2C bus
- SD Card SPI
- Primary SPI
- PWM support
- Arduino IDE support, with built-in RGB LED and IMU functions

---

## Getting started (Arduino IDE)

### 1. Install the board package

1. Install [Arduino IDE 2](https://www.arduino.cc/en/software).
2. Open **File → Preferences** and paste this URL in **Additional boards manager URLs**:

   ```
   https://raw.githubusercontent.com/yassinechouk/Elyssa-dev-board/main/elyssa-arduino/package_moovma_elyssa_index.json
   ```

3. Open **Tools → Board → Boards Manager**, search **Elyssa**, and click **Install**.

**Linux / macOS:** install Python 3.

### 2. Connect the board

1. Plug the board in with a USB-C **data** cable.
2. Select **Tools → Board → Moovma Elyssa → Elyssa**.
3. Select **Tools → Port →** the port labelled **Elyssa**.

Keep the other **Tools** options at their default values.

### 3. Upload your first sketch

1. Open **File → Examples → 01.Basics → Blink**.
2. Click **Upload**. The on-board LED starts blinking.

### If the upload fails

Put the board in download mode manually: **hold BOOT, press and release RESET, then release BOOT**.
Select the port that appears and click **Upload** again. This is usually needed only once.

### Serial Monitor

Open **Tools → Serial Monitor**. Press **RESET** to see the first messages.

---

## RGB LED

Built into the board package: no `#include` needed.

```cpp
setLedColor(ELYSSA_GREEN);     // ELYSSA_OFF, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
setLedRGB(255, 80, 0);         // any mix, 0..255 per channel
```

| Channel | Pin |
|---|---|
| Red | `LED_RED` = GPIO39 |
| Green | `LED_GREEN` = GPIO38 (`LED_BUILTIN`, so Blink works) |
| Blue | `LED_BLUE` = GPIO33 |

---

## Motion sensor (IMU)

The board has an STMicroelectronics LSM6DS3TR-C: accelerometer (in g) and gyroscope (in degrees per second). The basic functions are built into the board package, with no `#include`:

```cpp
void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) Serial.println("IMU not found");
  elyssa_imu_enable_tap();
}

void loop() {
  float x, y, z;
  elyssa_imu_read_accel(x, y, z);
  Serial.printf("x %.2f  y %.2f  z %.2f g\n", x, y, z);
  if (elyssa_imu_tapped()) setLedColor(ELYSSA_GREEN);
  delay(10);
}
```

Basic functions: reading (acceleration, rotation, temperature), ranges, sample rate, low power, pitch / roll / orientation, gyroscope calibration, tap and double tap, free-fall, motion, step counter, wake-up from deep sleep by motion.

Advanced functions (recording, filters, self-test, accelerometer calibration, stillness, tilt, wrist tilt, significant motion, interrupt pin...) are in the **ElyssaIMU** library, also included in the board package: add `#include <ElyssaIMU.h>`.

Examples: **File → Examples → ElyssaIMU** (Basics and Advanced). Full reference: [ElyssaIMU README](platform/libraries/ElyssaIMU/README.md). Known limitations: [TIPS_AND_KNOWN_ISSUES.md](TIPS_AND_KNOWN_ISSUES.md).

---

## Tools menu (defaults)

| Menu | Value |
|---|---|
| Board | Elyssa |
| Port | Elyssa on COMx |
| USB Mode | USB-OTG (TinyUSB) |
| USB CDC On Boot | Enabled |
| USB Firmware MSC On Boot | Disabled |
| USB DFU On Boot | Disabled |
| Upload Mode | USB-OTG CDC (TinyUSB) |
| CPU Frequency | 240MHz (WiFi) |
| Flash Mode | QIO 80MHz |
| Flash Size | 8MB (64Mb) |
| Partition Scheme | 8M with spiffs (3MB APP/1.5MB SPIFFS) |
| PSRAM | Disabled |
| Core Debug Level | None |
| Erase All Flash Before Sketch Upload | Disabled |
| JTAG Adapter | Disabled |

---

## USB modes

| | Mode 1 - Everyday (default) | Mode 2 - Debug (JTAG) |
|---|---|---|
| USB Mode | USB-OTG (TinyUSB) | Hardware CDC and JTAG |
| Upload Mode | USB-OTG CDC (TinyUSB) | UART0 / Hardware CDC |
| JTAG Adapter | Disabled | Integrated USB JTAG |

### Mode 1 → Mode 2

1. **USB Mode → Hardware CDC and JTAG**
2. **Upload**
3. **Upload Mode → UART0 / Hardware CDC**
4. **JTAG Adapter → Integrated USB JTAG**

### Mode 2 → Mode 1

1. **USB Mode → USB-OTG (TinyUSB)**
2. **Upload**
3. **Upload Mode → USB-OTG CDC (TinyUSB)**
4. **JTAG Adapter → Disabled**

If an upload fails: **hold BOOT, press and release RESET, release BOOT**, then **Upload** again.
