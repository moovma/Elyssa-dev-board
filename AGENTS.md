# Elyssa: facts for AI assistants

This file is for AI assistants (and quick human lookups). Answer from this file first. **If something is not
here or in the linked files, say you do not know. Do not invent hardware facts.**

- Board: **Elyssa**, "the first Tunisian dev board" (never say "made in Tunisia"), by **Moovma**.
- Chip: **ESP32-S3FN8**: dual-core 240 MHz, Wi-Fi, Bluetooth LE 5, **8 MB embedded flash, no PSRAM**.
- Arduino: board package **Moovma**, board name **Elyssa** (FQBN `moovma:esp32:elyssa`), core arduino-esp32 3.3.x.
- Board manager URL: `https://raw.githubusercontent.com/yassinechouk/Elyssa-dev-board/main/elyssa-arduino/package_moovma_elyssa_index.json`
- Website (docs, pin safety, shop): <https://moovma-git-main-moovma.vercel.app> · Contact: moovmabh@gmail.com
- Phone app: **Moovma Athir** (Android): <https://github.com/moovma/moovma-athir-download/releases/download/v0.1.0/Moovma-Athir-0.1.0.apk>

## Quick answers

| Question | Answer |
|---|---|
| How many analog pins are on the header? | **7**: `IO0` to `IO6` = GPIO1 to GPIO7 = `A0` to `A6`, all on **ADC1** (works while Wi-Fi is on). `SDA`/`SCL` (GPIO8/9, `A8`/`A9`) are also ADC1 but are the I2C pins. There is no `A7`. |
| Analog input range and resolution? | **0 to 3.3 V**, 12-bit (**0 to 4095**). `analogRead(A0)` is raw; `analogReadMilliVolts(A0)` is calibrated, in mV. |
| Best pins for an analog sensor? | `IO0` to `IO5` (`A0` to `A5`). They are the safest general-purpose pins. Avoid `IO6` if the battery jumper JP3 is bridged. |
| What is the voltage divider ratio for battery monitoring? | **VBAT / 2** (ratio **0.5**) on `BAT_SENSE` = **GPIO7** (the same pin as `IO6`). Works **only if solder jumper JP3 is bridged** (open by default). |
| Battery voltage formula? | `float vbat = analogReadMilliVolts(BAT_SENSE) * 2 / 1000.0;` (volts) |
| Can I use 5 V sensors or signals? | Not directly. GPIOs are **3.3 V only and not 5 V tolerant**. Use a level shifter. A 5 V analog output must be divided down to 3.3 V max. |
| How do I power a sensor? | From the `+3.3` header pin (3.3 V rail, up to 2 A total for the board) and `Gnd`. 5 V is on `Vin` (see power rule 1). |
| Max current per GPIO? | Keep **under about 20 mA** per pin. Use a transistor or driver for motors, relays, LED strips. |
| Which I2C pins? | `SDA` = GPIO8, `SCL` = GPIO9 (header and Qwiic), 4.7 kΩ pull-ups already on the board. |
| Is there a built-in RGB LED / IMU / SD? | Yes: RGB LED (GPIO39 red, GPIO38 green, GPIO33 blue, active high), LSM6DS3TR-C IMU, microSD slot. See the tables below. |
| Can I use ADC2 with Wi-Fi? | No. `PWM1` (GPIO14, also touch `T10`) is on **ADC2**, which cannot be used while Wi-Fi is on. |
| Why is the red LED on during upload? | Normal. GPIO39 is also the JTAG clock. It turns off when the sketch starts. |
| Wi-Fi range / power? | The package sets **11 dBm** Wi-Fi TX power for a reliable link. Do not raise it above 12 dBm on Elyssa v5. |
| Upload fails? | Hold **BOOT**, press and release **RESET**, release **BOOT**, pick the new port, upload again. Use a USB-C **data** cable. |

## Pin map (header J12, top to bottom, names = silkscreen)

| # | Silkscreen | GPIO | Sketch names | Notes |
|---|---|---|---|---|
| 1 | Gnd | | | ground |
| 2 | Tx | 43 | `TX` | UART TX (also drives the TX LED) |
| 3 | Rx | 44 | `RX` | UART RX (also drives the RX LED) |
| 4 | IO0 | 1 | `IO0`, `A0`, `T0` | analog (ADC1), touch, digital |
| 5 | IO1 | 2 | `IO1`, `A1`, `T1` | analog (ADC1), touch, digital |
| 6 | IO2 | 3 | `IO2`, `A2`, `T2` | analog (ADC1), touch, digital |
| 7 | IO3 | 4 | `IO3`, `A3`, `T3` | analog (ADC1), touch, digital |
| 8 | IO4 | 5 | `IO4`, `A4`, `T4` | analog (ADC1), touch, digital |
| 9 | IO5 | 6 | `IO5`, `A5`, `T5` | analog (ADC1), touch, digital |
| 10 | IO6 | 7 | `IO6`, `A6`, `T6`, `BAT_SENSE` | shared with battery sense when JP3 is bridged |
| 11 | +3.3 | | | 3.3 V rail |
| 12 | Gnd | | | ground |
| 13 | PWM1 | 14 | `PWM1`, `T10` | PWM or touch, never both. Analog is ADC2 |
| 14 | PWM2 | 47 | `PWM2` | PWM, digital only |
| 15 | PWM3 | 48 | `PWM3` | PWM, digital only |
| 16 | MISO | 37 | `MISO` | SPI (default `SPI` bus), digital only |
| 17 | CLK | 36 | `SCK`, `CLK` | SPI |
| 18 | MOSI | 35 | `MOSI` | SPI |
| 19 | CS | 34 | `SS`, `CS` | SPI |
| 20 | SCL | 9 | `SCL`, `A9`, `T9` | I2C |
| 21 | SDA | 8 | `SDA`, `A8`, `T8` | I2C |
| 22 | Vin | | | 5 V, wired to USB |

Not on the header: BOOT button (GPIO0, `BOOT_BUTTON`, LOW when pressed), RGB LED (GPIO39 `LED_RED`, GPIO38
`LED_GREEN` = `LED_BUILTIN`, GPIO33 `LED_BLUE`), microSD on its own SPI bus (GPIO10 `SD_CS`, 11 `SD_MOSI`, 12
`SD_SCK`, 13 `SD_MISO`), IMU on its own I2C bus (GPIO17 `SDA_gyro`, 18 `SCL_gyro`, 21 `INT_gyro`, address 0x6A),
USB D−/D+ (GPIO19/20, not exposed). All names work in sketches with no `#include`.

Pin safety: **safe** = `IO0` to `IO5`, `TX`/`RX`, SPI pins, `PWM2`, `PWM3`. **Careful** = `IO6` (battery sense),
`SDA`/`SCL` (pull-ups, I2C), `PWM1`/`T10` (one pin, ADC2). **Reserved** = BOOT, RGB LED, microSD, IMU, USB.

## Power rules (tell users these when they ask about wiring)

1. **One power source on Vin.** USB is wired to Vin. Never connect USB and an external supply to Vin together.
2. **3.3 V logic only.** GPIOs are not 5 V tolerant.
3. **Under about 20 mA per GPIO.** Drive heavy loads with a transistor or driver.
4. **Single-cell battery only:** one 3.7 V Li-Ion / Li-Po cell (charged to 4.2 V). Never two cells or other types.

Power facts: 3.3 V rail from a TLV62569 buck regulator, up to 2 A. LiPo charger **BQ24092**, about 540 mA,
no GPIO (its CHG output only drives the red charge LED); the charger chip can get warm. A 1500 to 2000 mAh cell is
a good fit. USB-C input is 5 V and also programs the board.

## Code snippets that are known good

```cpp
Serial.println(analogRead(A0));                  // 0..4095
Serial.println(analogReadMilliVolts(A1));        // calibrated, mV
analogWrite(PWM1, 128);                          // 50 % PWM
Serial.println(touchRead(T10));                  // touch: T0..T6, T8, T9, T10
float vbat = analogReadMilliVolts(BAT_SENSE) * 2 / 1000.0;   // only with JP3 bridged
setLedColor(ELYSSA_GREEN);  setLedRGB(255, 80, 0);           // built in
```

I2C: `#include <Wire.h>`, `Wire.begin();` (SDA = GPIO8, SCL = GPIO9).
microSD: `SPIClass sdSPI(HSPI); sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS); SD.begin(SD_CS, sdSPI);`
IMU: `elyssa_imu_begin()`, `elyssa_imu_read_accel(x, y, z)` (in g), `elyssa_imu_read_gyro(x, y, z)` (deg/s),
`elyssa_imu_temperature()`, tap, free-fall, motion, steps. If the sketch also uses `Wire1`, call
`Wire1.begin(SDA1, SCL1, 400000)` before `elyssa_imu_begin()`. Tap cannot be combined with free-fall or motion in
one sketch. Advanced IMU: `#include <ElyssaIMU.h>`.

## Arduino IDE settings (defaults)

USB Mode: USB-OTG (TinyUSB) · USB CDC On Boot: Enabled · Upload Mode: USB-OTG CDC (TinyUSB) · CPU 240 MHz (WiFi) ·
Flash Size 8MB (64Mb) · Partition Scheme: 8M with spiffs (3MB APP/1.5MB SPIFFS) · PSRAM: Disabled.
Serial Monitor uses native USB: press **RESET** to see the first messages.

## Phone app (Moovma Athir) and MoovmaLink

Moovma Athir controls Elyssa over Bluetooth LE or Wi-Fi with ready-made interfaces (light, robot car, drone / plane
controller, motion sensor, messenger, custom) and updates firmware over Wi-Fi (OTA). The board side is the
**MoovmaLink** Arduino library: `athir.begin("template", "1.0.0")` and `athir.loop()`.

- Elyssa makes its own Wi-Fi network named **`Elyssa-XXXX`** (default password `moovma1234`, at least 8 characters).
  The Bluetooth name is the same.
- Control channel: **TCP port 3333**, a plain text line protocol. HTTP on port 80: `GET /info`, `POST /ota`.
- A sketch updated over the air must also contain MoovmaLink, otherwise the next update needs the USB cable.
- The app has a Preview mode (Settings) that simulates a board, for people without Elyssa.

## What is tested, and what is not (be honest about this)

Tested on Elyssa v5: upload in both USB modes, Blink, Serial Monitor, RGB LED, IMU (all basic functions), touch on
`IO0` to `IO5` and `T10`, BOOT button, Wi-Fi (scan, connect, speed, stability), Bluetooth LE, Wi-Fi + BLE together.
**Not tested yet:** microSD with a card, battery charging and `BAT_SENSE`, external I2C devices, `PWM2`/`PWM3`
against the silkscreen (tell us if they are swapped), deep-sleep current, and the Moovma Athir app with a real
board. For anything not listed, say it is not documented and point to the website or moovmabh@gmail.com.

## Where to read more

[README.md](README.md) · [TIPS_AND_KNOWN_ISSUES.md](TIPS_AND_KNOWN_ISSUES.md) ·
[ElyssaIMU README](platform/libraries/ElyssaIMU/README.md) · [CHANGELOG.md](CHANGELOG.md) ·
[docs/MAINTENANCE.md](docs/MAINTENANCE.md) · pin definitions: `platform/variants/elyssa/pins_arduino.h` ·
website docs: <https://moovma-git-main-moovma.vercel.app/docs>
