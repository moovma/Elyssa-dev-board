# Maintenance guide

For the people who build and publish the Moovma board package. Users do not need this.

## What is where

| Path | Content |
|---|---|
| `platform/boards.txt` | the Elyssa board and its Tools menus |
| `platform/platform.txt` | build and upload recipes (Espressif's file, with a few Moovma changes marked `Moovma`) |
| `platform/variants/elyssa/` | `pins_arduino.h`, `variant.cpp` (RGB LED and IMU core functions), `elyssa_wifi_power.c` (Wi-Fi power limit), ST's LSM6DS3TR-C driver, `bootloader_qio_80m.bin`, `partitions_bin/` |
| `platform/libraries/ElyssaIMU/` | the ElyssaIMU library and its 16 examples |
| `elyssa-arduino/package_moovma_elyssa_index.json` | the index read by the Boards Manager |
| `CHANGELOG.md`, `README.md`, `TIPS_AND_KNOWN_ISSUES.md` | documentation |

The files in `platform/` are the ones that differ from Espressif's arduino-esp32 core. The release archive contains the whole package (core, tools and these files).

Versions used by 1.1.2: arduino-esp32 core **3.3.12** (ESP-IDF 5.5.5), esptool **5.3.1**, compiler `esp-x32` **2601**.

## Rules for `boards.txt`

- In every menu, the **first option is the default**.
- Menu **option keys are a contract** (for example `8M`, `cdc`, `qio`). Renaming or removing one breaks the settings saved by users ("Invalid FQBN"). This is why the Flash Size, Flash Mode and PSRAM menus keep a single entry.
- `upload_port.0.vid` / `pid` must match `USB_VID` / `USB_PID` in `pins_arduino.h`.

## USB identity

The VID is Espressif's `0x303A`. The PID `0x81FF` is **temporary**: replace it with the PID assigned by Espressif (request it at github.com/espressif/usb-pids) in **both** `pins_arduino.h` and `boards.txt`. In "Hardware CDC and JTAG" mode the identity is fixed inside the chip (`303A:1001`), so it never changes.

## The files that speed up compiles (1.1.2)

Three steps that always gave the same result were removed from every compile:

| File | Replaces | How `platform.txt` / `boards.txt` use it |
|---|---|---|
| `variants/elyssa/bootloader_qio_80m.bin` | `esptool elf2image` of `bootloader_qio_80m.elf` | `elyssa.build.custom_bootloader=bootloader_qio_80m` (Espressif's own "custom bootloader" mechanism: a `bootloader.bin` in the sketch folder still wins) |
| `variants/elyssa/partitions_bin/<scheme>.bin` | `gen_esp32part` on the scheme's `.csv` | copied unless the sketch or the variant has its own `partitions.csv`, in which case `gen_esp32part` runs as before |
| (nothing) | `merge-bin` creating the 8 MB `merged.bin` | removed, see the comment in `platform.txt` |

### When the ESP32 core is updated: redo these

1. **Re-apply the Moovma changes to the new `platform.txt`** (search for `Moovma`): the partition recipe (both lines), the removed `merge-bin` lines, the name, and `"-Wl,--wrap=esp_wifi_start"` in `compiler.c.elf.flags` (without it the Wi-Fi power limit silently stops working). Check that the "custom bootloader" lines still exist in the new file.
2. **Regenerate the bootloader** from the new core's `esp32s3-libs`:
   ```
   esptool --chip esp32s3 elf2image --flash-mode dio --flash-freq 80m --flash-size 8MB -o bootloader_qio_80m.bin <esp32s3-libs>/bin/bootloader_qio_80m.elf
   ```
   Check it by compiling once **without** the precomputed file and comparing the `<sketch>.ino.bootloader.bin` hash with the new file.
3. **Regenerate the 4 partition tables** (`default_8MB`, `default_ffat_8MB`, `large_spiffs_8MB`, `max_app_8MB`) from the `.csv` files of the new core:
   ```
   python gen_esp32part.py -q tools/partitions/<scheme>.csv partitions_bin/<scheme>.bin
   ```
4. Run the **test list** below.

Reference values for core 3.3.12 (SHA-256 starts with): bootloader `883BB1E4`, `default_8MB` table `1D9CCA96`.

## Publishing a release

Order matters: **push the commit first, then create the release**, so the tag lands on the right commit.

1. **Prepare the files** in `platform/`, update `CHANGELOG.md` and the docs.
2. **Build the archive.** Take the previous release archive, replace the changed files with the ones from `platform/`, keep the top-level folder `elyssa-X.Y.Z/`, and zip it (on Linux: `zip -qr -X elyssa-X.Y.Z.zip elyssa-X.Y.Z`; use zip or 7-Zip, not `Compress-Archive` of Windows PowerShell 5.1, which can write backslashes in paths).
3. **Compute the checksum and size:**
   ```powershell
   (Get-FileHash .\elyssa-X.Y.Z.zip -Algorithm SHA256).Hash
   (Get-Item .\elyssa-X.Y.Z.zip).Length
   ```
4. **Add the entry in the index**: copy the previous entry and change `version`, `url`, `archiveFileName`, `checksum` (`SHA-256:<hash in lower case>`) and `size` (a string, in bytes). The `name` is `Moovma` in every entry.
5. **Test locally** (see below), then **commit and push**.
6. **Create the GitHub release**: tag `vX.Y.Z` on `main`, title `Elyssa X.Y.Z`, the `CHANGELOG.md` section as description, and attach `elyssa-X.Y.Z.zip` exactly as built.
7. **Verify what users get**: download the archive from the release and compare its SHA-256 and size with the index:
   ```powershell
   Invoke-WebRequest "https://github.com/yassinechouk/Elyssa-dev-board/releases/download/vX.Y.Z/elyssa-X.Y.Z.zip" -OutFile "$env:TEMP\check.zip"
   (Get-FileHash "$env:TEMP\check.zip" -Algorithm SHA256).Hash
   ```
   Until the release exists, the index points to a file that cannot be downloaded: do steps 5 and 6 close together.
8. **Install it the way a user does**: close the IDE, delete `%LOCALAPPDATA%\Arduino15\package_moovma_elyssa_index.json`, reopen the IDE and install from the Boards Manager.

Never modify an archive that is already published (installed copies and cached downloads would no longer match the checksum): publish a new version instead.

## Test list before a release

On a real Elyssa board, with the IDE:

1. **Blink** compiles and uploads; the Serial Monitor works.
2. A **Wi-Fi** sketch and a **Bluetooth** sketch compile.
3. A sketch folder with its **own `partitions.csv`** gives the partition table of that file (print the partitions with `esp_partition_find`).
4. The compile log shows the bootloader and the partition table being **copied** (no `esptool ... elf2image` for the bootloader, no `merge-bin`).
5. The hashes of `<sketch>.ino.bootloader.bin` and `<sketch>.ino.partitions.bin` equal the reference values above.
6. **USB Mode 2** (Hardware CDC and JTAG) uploads.
7. RGB LED and IMU sketches work; the LED is off after upload.
8. A Wi-Fi sketch prints `44` (11 dBm) for `esp_wifi_get_max_tx_power()` after `WiFi.mode(WIFI_STA)` and again after connecting, and Blink has the same size as before. (`WiFi.getTxPower()` returns a fixed 78 until the core has processed the "station started" event, so do not use it right after `WiFi.mode()`.)

## After editing the installed package by hand

When you test changes by copying files into `%LOCALAPPDATA%\Arduino15\packages\moovma\hardware\esp32\<version>\`, close the IDE and clear `%LOCALAPPDATA%\arduino\cores` and `%LOCALAPPDATA%\arduino\sketches` if you changed a header of the variant.

## The ST driver

`variants/elyssa/lsm6ds3tr-c_reg.c/.h` is STMicroelectronics' official driver (BSD-3-Clause, `LICENSE_ST.txt`). It is used unchanged; the workarounds for its bugs are in `variant.cpp` and in the ElyssaIMU library (see `TIPS_AND_KNOWN_ISSUES.md`).
