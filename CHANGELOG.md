# Changelog

## 1.1.3

### Fixed
- **Wi-Fi on Elyssa v5: transmit power limited to 11 dBm.** The Elyssa v5 Wi-Fi transmitter gives a clean signal only up to about 12 to 14 dBm, depending on the router and the channel. At the ESP32 default of 20 dBm the router could not decode the board's frames: connecting often failed (reason 2 `AUTH_EXPIRE`) or the speed fell to about 1 Mbit/s. Measured on the board (9 runs, 2 routers): full speed (about 22 Mbit/s) at 11 dBm on every run, speed dropping from 12 dBm on the worst router and 14 dBm on the best, under 10 % of full speed from 13.5 to 15 dBm, no connection at 19 to 20 dBm; reception is not affected. The board package now sets 11 dBm (2.5 dB below the lowest collapse measured) every time Wi-Fi starts (station, access point, ESP-NOW), so every sketch works without change. The range is lower than a board working at 20 dBm (about one wall less indoors). Sketches without Wi-Fi are not affected (same size).
- **Compiling on Linux and macOS** failed after "Successfully created ESP32-S3 image" with `unexpected EOF while looking for matching '"'` (quotes in the partition table recipe). Windows was not affected.

Bluetooth LE is not limited: its default power (+9 dBm) works, and connection, scan and Wi-Fi + Bluetooth coexistence were tested on the board.

## 1.1.2

### Faster compiles
Each compile used to launch 3 extra helper programs (esptool / gen_esp32part, about 1 s each on Windows) whose result never changes:
- **Bootloader:** precomputed image in `variants/elyssa` (`bootloader_qio_80m.bin`), identical to what esptool 5.3.1 produces from the arduino-esp32 3.3.12 bootloader.
- **Partition table:** precomputed for the 4 Elyssa schemes in `variants/elyssa/partitions_bin`. `gen_esp32part` still runs when a sketch (or the variant) has its own `partitions.csv`.
- **Merged image:** the 8 MB `<sketch>.merged.bin` is no longer created (the IDE upload doesn't use it). It can still be made by hand with `esptool merge-bin`.

### Changed
- **Flash mode fixed to QIO 80 MHz** (the embedded flash of the ESP32-S3FN8). The DIO option was removed from Tools > Flash Mode.

Nothing changes in the sketch, the libraries or the upload.

## 1.1.1

### Added
- `T10` touch pin = GPIO14 (the PWM1 pad). T10 and PWM1 are the same pin: use it for touch or for PWM.

### Fixed
- RGB LED: the red LED no longer glows dimly when a sketch doesn't use the LED. The red channel is on GPIO39 (JTAG MTCK), which has an internal pull-up at reset; the board package now turns the 3 LED pins off at startup (`initVariant()`). The LED is still red during an upload (the chip's ROM bootloader is running, no sketch code can act then).

### Changed
- Package name **Moovma** (Boards Manager and Tools > Board menu), to host future Moovma boards. The board is still called Elyssa.

## 1.1.0

Board package for Elyssa v5. Existing sketches keep working.

### Added
- **RGB LED** support: `LED_RED` (GPIO39), `LED_GREEN` (GPIO38, `LED_BUILTIN`), `LED_BLUE` (GPIO33), `setLedColor()` with named colours (`ELYSSA_RED`...), `setLedRGB()` for any mix.
- **IMU core functions** built into the board package (no `#include`): `elyssa_imu_begin()`, reading (`elyssa_imu_read_accel()`, `elyssa_imu_read_gyro()`, `elyssa_imu_accel_x()`..., `elyssa_imu_temperature()`), ranges, sample rate, low power, pitch / roll / orientation, gyroscope calibration, tap / double tap, free-fall, motion, step counter, wake-up from deep sleep by motion.
- **ElyssaIMU library 2.0.2**, bundled: recording (FIFO), filters, self-test, accelerometer calibration, timestamp, stillness, orientation events, advanced steps, tilt, wrist tilt, significant motion, interrupt pin and callbacks, fine tuning. 16 examples (File → Examples → ElyssaIMU).
- **STMicroelectronics LSM6DS3TR-C driver** (official, BSD-3-Clause) in the variant, with workarounds for 3 driver bugs (`tilt_src_set`, `motion_sens_set(0)`, `timestamp_set(0)`).

### Changed
- IMU driver for the **LSM6DS3TR-C** (Elyssa v5), WHO_AM_I 0x6A.
- IMU readings use the current range (previously always +-2 g / +-250 dps).
- The first 10 samples after start are discarded (gyroscope settling).
- `boards.txt`: USB modes documentation; GPIO39 is now the red LED.

### Compatibility
- Older names still work: `accel_return_ax()`, `gyro_return_ax()`, `elyssa_imu_read(gyro, accel, &temp)`, `elyssa_imu_whoami()`, `elyssa_imu_read_reg()`.

### Validated on Elyssa v5
- IMU core test: 61/61 + wake-up from deep sleep.
- ElyssaIMU advanced test: 90/93 (wrist-tilt direction: see TIPS_AND_KNOWN_ISSUES.md).

## 1.0.1
- Core arduino-esp32 3.3.12, esptool 5.3.1.

## 1.0.0
- First release.
