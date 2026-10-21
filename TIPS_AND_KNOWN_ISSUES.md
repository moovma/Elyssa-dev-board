# Tips and known issues

## IMU

### Using Wire1 and the IMU in the same sketch
The IMU uses I2C controller 1, the same as `Wire1`. If your sketch also uses `Wire1`, call `Wire1.begin(SDA1, SCL1, 400000)` **before** `elyssa_imu_begin()`.

### Tap can't be combined with free-fall or motion
The IMU has a single "latched events" setting. Tap only works with it off, free-fall and motion turn it on. The last `elyssa_imu_enable_...()` call wins. Use tap in one sketch, free-fall / motion in another, or switch between them.

### Tap: check often
Without the latch, a tap is visible for about 29 ms. Call `elyssa_imu_tapped()` / `elyssa_imu_double_tapped()` often: no long `delay()` in `loop()`.

### The gyroscope shows a small rotation at rest
Normal for every gyroscope (a few dps on Elyssa, it changes with temperature). Call `elyssa_imu_calibrate_gyro()` with the board still. Changing the gyroscope range clears the calibration.

### Some features change the sample rate
Tap sets 416 Hz. The step counter, tilt, wrist tilt and significant motion need 26 Hz or more.

### Strong smoothing reacts slowly
`ELYSSA_ACCEL_SMOOTH_MAX` at 104 Hz needs about 3 s to settle after being turned on.

### Tap direction is approximate
`elyssa_imu_tap_direction()` gives the axis that shook the most. A tap on an edge can also shake the board vertically (Z).

### Wrist tilt: use the edges, not Z+
With Z+ enabled in `elyssa_imu_set_wrist_tilt_axes()`, simply lying flat face up triggers the gesture. Use X+-, Y+-.

### Known issue: wrist-tilt direction
`elyssa_imu_wrist_tilted()` works, but `elyssa_imu_wrist_tilt_direction()` can return `ELYSSA_DIRECTION_NONE`, and holding the position can count a second event. Workaround: after a wrist tilt, use `elyssa_imu_orientation()` to know which edge is down. Under investigation.

### Chip and ST driver behaviours handled by the board package
Tested on Elyssa v5:
- The accelerometer user offset is subtracted on Z (added on X and Y); the library corrects the sign.
- TAP_IA is never set when events are not latched; the direction bits are read without it.
- ST driver bugs worked around: `lsm6ds3tr_c_tilt_src_set()` reads instead of writing; `lsm6ds3tr_c_motion_sens_set(0)` and `lsm6ds3tr_c_timestamp_set(0)` never write the register.

### "multiple definition of lsm6ds3tr_c_..."
Another installed library contains ST's LSM6DS3TR-C driver. The Elyssa board package already includes it: remove the other library.

## RGB LED

### Red during upload
The red LED lights up while a sketch is being uploaded. This is normal: the red channel is on GPIO39, which is also the JTAG clock pin (MTCK) and has an internal pull-up at reset. During an upload the chip runs its ROM bootloader, so no sketch code can turn it off. As soon as the new sketch starts, the board package turns the LED off (since 1.1.1).

### Colours look unbalanced
The green channel looks brighter than red and blue (same 330 ohm resistors, different LED efficiencies). Use `setLedRGB()` to balance mixed colours.

## Wi-Fi

### Wi-Fi transmit power is limited to 11 dBm (since 1.1.3)
On Elyssa v5 the Wi-Fi transmitter gives a clean signal only up to about 12 to 14 dBm, depending on the router and the channel. At the ESP32 default of 20 dBm, the router cannot decode the board's frames: the connection fails (reason 2 `AUTH_EXPIRE`) or the speed falls to about 1 Mbit/s. The board package therefore sets **11 dBm** each time Wi-Fi starts (`WiFi.begin()`, `WiFi.softAP()`, ESP-NOW...). Nothing to add in your sketch.

What it means:
- Full speed (about 22 Mbit/s), a reliable connection and no data loss, with a 2.5 dB margin below the lowest limit measured.
- Less range than a board at 20 dBm: about one wall less indoors, about half the distance in open air.
- Reception is not affected.

To choose another power anyway, call `WiFi.setTxPower()` **after** `WiFi.mode()` and before `WiFi.begin()`:
```cpp
WiFi.mode(WIFI_STA);
WiFi.setTxPower(WIFI_POWER_8_5dBm);   // lower: less current, less range
WiFi.begin(ssid, password);
```
Values above 12 dBm can bring back the problem on Elyssa v5 (slower speed, longer delays), depending on the router; above 14 dBm it comes back on every router tested. The setting stays until Wi-Fi is stopped (`WiFi.mode(WIFI_OFF)`); when Wi-Fi starts again, the package sets 11 dBm again.

### Wi-Fi and Bluetooth at the same time
They share one radio. Both work together, but Wi-Fi uses power save while Bluetooth is on (ping about 20 ms instead of 2 ms), and Bluetooth hears fewer devices while Wi-Fi sends a lot of data.

## Pins

### `T10` and `PWM1` are the same pin
Both are GPIO14. Use it for touch **or** for PWM, not both at the same time.

### `IO6` is also the battery sense pin
`BAT_SENSE` is GPIO7, the same pin as `IO6`. It only works if solder jumper **JP3** is bridged (open by default). When JP3 is bridged, do not use `IO6` for anything else.

### `PWM2` and `PWM3` follow the silkscreen
The pin printed `PWM2` on the header is GPIO47 (`PWM2`) and the pin printed `PWM3` is GPIO48 (`PWM3`). If you find them swapped on your board, please tell us.

### No `A7` and no `T7`
The numbering skips 7. `A8` / `A9` and `T8` / `T9` are the I2C pins (GPIO8 / GPIO9): avoid touch on them when you use I2C, the 4.7 kΩ pull-ups are on the board.

### The port name depends on the USB mode
In **USB-OTG (TinyUSB)** mode (default) the port is labelled **Elyssa**. In **Hardware CDC and JTAG** mode the USB identity is fixed inside the chip, so the port appears as **USB JTAG/serial debug unit**.

## Compile times

| Situation | Typical time (our Windows test PC) |
|---|---|
| First compile after installing or updating the package | 1 to 4 minutes (the ESP32 core is rebuilt once) |
| First compile of a **new** sketch | about 40 to 90 s, longer with big libraries |
| Compiling the **same** sketch again | about 7 to 9 s |

### Why a new sketch is slow
The Arduino IDE keeps the compiled ESP32 core and the compiled files of **each sketch**. A new sketch therefore compiles again its libraries (Wi-Fi, Bluetooth, ElyssaIMU...) and the two Elyssa files of the board package. Before compiling, the IDE also reads every file of every library it finds, and a library with many files costs time: the built-in `BLE` library has about 30 files, the external `NimBLE-Arduino` library about 200.

### What helps
- **Keep working in the same saved sketch** (in your sketchbook folder) instead of creating a new one each time. Each sketch folder keeps its own compiled files.
- **Do not clear the IDE cache** (`%LOCALAPPDATA%\arduino\cores` and `...\sketches`) unless you edited the board package by hand. Clearing it brings back the 1 to 4 minute core rebuild.
- **Prefer the built-in libraries** (`BLE`, `WiFi`) to big external ones when both can do the job.
- **Windows: exclude the Arduino folders from your antivirus scan.** Compiling creates thousands of small files, and every one of them is scanned. In a PowerShell window **opened as administrator**, with the IDE closed (this tells Windows Defender not to scan the Arduino folders and tools; do it only if you are comfortable with that):
  ```powershell
  Add-MpPreference -ExclusionPath "$env:LOCALAPPDATA\arduino", "$env:LOCALAPPDATA\Arduino15", "$env:USERPROFILE\Arduino"
  Add-MpPreference -ExclusionProcess "xtensa-esp32s3-elf-g++.exe", "xtensa-esp32s3-elf-gcc.exe", "esptool.exe", "gen_esp32part.exe", "arduino-cli.exe", "ctags.exe"
  ```
  With another antivirus, add the same folders there.
- **Turn off verbose output** (**File → Preferences → Show verbose output during: compile**) and set **Compiler warnings** to **None** or **Default**.
- **On a laptop, plug in the charger** and use the "Best performance" power mode.

## Build and upload

### `merged.bin` is no longer created (since 1.1.2)
The IDE does not use it for uploading (it sends the bootloader, the partition table and the program separately), and creating it slowed every compile. **Sketch → Export Compiled Binary** still gives you the separate files in the `build` folder next to your sketch. If you need a single file, for example for a web flashing tool, create it with esptool (Windows path shown, one command, replace `sketch` by your sketch name):
```
"%LOCALAPPDATA%\Arduino15\packages\moovma\tools\esptool_py\5.3.1\esptool.exe" --chip esp32s3 merge-bin -o merged.bin --pad-to-size 8MB --flash-mode keep --flash-freq keep --flash-size keep 0x0 sketch.ino.bootloader.bin 0x8000 sketch.ino.partitions.bin 0xe000 boot_app0.bin 0x10000 sketch.ino.bin
```
`boot_app0.bin` is in the `tools\partitions` folder of the installed board package.

### Flash Mode is always QIO 80 MHz (since 1.1.2)
It is the mode of the ESP32-S3FN8 embedded flash. The Flash Mode menu has a single entry so that settings saved by older versions keep working.

### Custom partition table
Put a `partitions.csv` file in your sketch folder: it is used instead of the **Partition Scheme** menu.

### Step-by-step debugging
**USB Mode 2** (Hardware CDC and JTAG) works for uploading and for the serial output. Debugging with breakpoints from the IDE is not available yet: the OpenOCD and GDB tools are not part of the package.
